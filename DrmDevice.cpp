#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/kd.h>
#include <cassert>

#include "DrmDevice.hpp"

DrmDevice::~DrmDevice()
{
    close();
}

int DrmDevice::open()
{
    char devicePath[16];

    sprintf(devicePath, "/dev/dri/card%d", cardNumber);
    int fd = ::open(devicePath, O_RDWR);
    if (fd < 0) {
        cerr << "Error opening " << devicePath << endl;
        return -1;
    }

    uint64_t hasDumbBuffers;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &hasDumbBuffers) < 0 || !hasDumbBuffers) {
        cerr << "DRM device " << devicePath << " does not support dumb buffers" << endl;
        return -1;
    }

    return fd;
}

int DrmDevice::prepare(int fd)
{
    drmModeRes *cardResources;
    drmModeConnector *connector;
    unsigned int i;
    struct modeset_dev *dev;
    int ret;

    cardResources = drmModeGetResources(fd);
    if (!cardResources) {
        fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n",
            errno);

        return -errno;
    }

    cout << "Found #connectors: " << cardResources->count_connectors << ", #encoders: " << cardResources->count_encoders << endl;
    
    for (i = 0; i < cardResources->count_connectors; ++i) {
        connector = drmModeGetConnector(fd, cardResources->connectors[i]);
        if (!connector) {
            fprintf(stderr, "cannot retrieve DRM connector %u:%u (%d): %m\n",
                i, cardResources->connectors[i], errno);

            continue;
        }

        dev = (modeset_dev *)malloc(sizeof(*dev));
        memset(dev, 0, sizeof(*dev));
        dev->conn = connector->connector_id;

        /* call helper function to prepare this connector */
        ret = setupDevice(fd, cardResources, connector, dev);
        if (ret) {
            if (ret != -ENOENT) {
                errno = -ret;
                fprintf(stderr, "cannot setup device for connector %u:%u (%d): %m\n",
                    i, cardResources->connectors[i], errno);
            }

            free(dev);
            drmModeFreeConnector(connector);

            continue;
        }

        cout << "Adding connector " << connector->connector_id << " to the list" << endl;
        
        drmModeFreeConnector(connector);
        dev->next = modeset_list;
        modeset_list = dev;
    }

    drmModeFreeResources(cardResources);

    return 0;
}

int DrmDevice::setupDevice(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
    int ret;

    if (conn->connection != DRM_MODE_CONNECTED) {
        fprintf(stderr, "ignoring unused connector %u\n", conn->connector_id);

        return -ENOENT;
    }

    // Make sure there's at least one valid mode
    if (conn->count_modes == 0) {
        fprintf(stderr, "no valid mode for connector %u\n", conn->connector_id);

        return -EFAULT;
    }

    // Copy mode[0] information into local structure
    // Basically I just use the first available mode for the connector, which normally is the one with higher resolution
    memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
    dev->width = conn->modes[0].hdisplay;
    dev->height = conn->modes[0].vdisplay;

    printf("mode[0] for connector %u is %ux%u (has %d modes)\n",
           conn->connector_id, dev->width, dev->height, conn->count_modes);

    for (unsigned int i = 1; i < conn->count_modes; i++) {
        drmModeModeInfo mode = conn->modes[i];
        cout << "   mode[" << i << "] " << mode.hdisplay << "x" << mode.vdisplay << "@" << mode.vrefresh << endl;
    }
    
    // Look for CRTC for the connector
    ret = findCrtc(fd, res, conn, dev);
    if (ret) {
        fprintf(stderr, "no valid crtc for connector %u\n",
            conn->connector_id);

        return ret;
    }

    // Finally create a framebuffer for the CRTC
    ret = createFB(fd, dev);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer for connector %u\n",
            conn->connector_id);

        return ret;
    }

    return 0;
}

int DrmDevice::findCrtc(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
    drmModeEncoder *enc;
    unsigned int i, j;
    int32_t crtc;
    struct modeset_dev *iter;

    /* first try the currently conected encoder+crtc */
    if (conn->encoder_id)
        enc = drmModeGetEncoder(fd, conn->encoder_id);
    else
        enc = NULL;

    if (enc) {
        if (enc->crtc_id) {
            crtc = enc->crtc_id;
            for (iter = modeset_list; iter; iter = iter->next) {
                if (iter->crtc == crtc) {
                    crtc = -1;
                    break;
                }
            }

            if (crtc >= 0) {
                drmModeFreeEncoder(enc);
                dev->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    /* If the connector is not currently bound to an encoder or if the
        * encoder+crtc is already used by another connector (actually unlikely
        * but lets be safe), iterate all other available encoders to find a
        * matching CRTC. */
    for (i = 0; i < conn->count_encoders; ++i) {
        enc = drmModeGetEncoder(fd, conn->encoders[i]);
        if (!enc) {
            fprintf(stderr, "cannot retrieve encoder %u:%u (%d): %m\n",
                i, conn->encoders[i], errno);
            continue;
        }

        /* iterate all global CRTCs */
        for (j = 0; j < res->count_crtcs; ++j) {
            /* check whether this CRTC works with the encoder */
            if (!(enc->possible_crtcs & (1 << j)))
                continue;

            /* check that no other device already uses this CRTC */
            crtc = res->crtcs[j];
            for (iter = modeset_list; iter; iter = iter->next) {
                if (iter->crtc == crtc) {
                    crtc = -1;
                    break;
                }
            }

            /* we have found a CRTC, so save it and return */
            if (crtc >= 0) {
                drmModeFreeEncoder(enc);
                dev->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    fprintf(stderr, "cannot find suitable CRTC for connector %u\n",
        conn->connector_id);

    return -ENOENT;
}

int DrmDevice::createFB(int fd, struct modeset_dev *dev)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;
    int ret;

    /* create dumb buffer */
    memset(&creq, 0, sizeof(creq));
    creq.width = dev->width;
    creq.height = dev->height;
    creq.bpp = 32;
    ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        fprintf(stderr, "cannot create dumb buffer (%d): %m\n",
            errno);
        return -errno;
    }
    dev->stride = creq.pitch;
    dev->size = creq.size;
    dev->handle = creq.handle;

    /* create framebuffer object for the dumb-buffer. 
     * Since dev->width and height are from the first mode available, the framebuffer is created for this size,
     * and its id stored in dev->fb
     */
    ret = drmModeAddFB(fd, dev->width, dev->height, 24, 32, dev->stride, dev->handle, &dev->fb);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_destroy;
    }

    /* prepare buffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = dev->handle;
    ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        fprintf(stderr, "cannot map dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }

    // The framebuffer location is now pointed in memory at location dev->map
    dev->map = (uint8_t *)mmap(0, dev->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
    if (dev->map == MAP_FAILED) {
        fprintf(stderr, "cannot mmap dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }

    // Clear all framebuffer space to 0, equivalent to blanking the screen
    if (!memset(dev->map, 0, dev->size)) {
        cerr << "Could not clear framebuffer" << endl;
        goto err_fb;
    }
    
    cout << "fb (id: " << dev->fb << ") created " << creq.width << "x" << creq.height 
         << ", Bytes size: " << dev->size <<  endl;

    return 0;

err_fb:
    drmModeRmFB(fd, dev->fb);
    cerr << "Error creating fb" << endl;

err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = dev->handle;
    drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

    return ret;
}

int DrmDevice::setMode(int fd)
{
    struct modeset_dev *iter;

    for (iter = modeset_list; iter; iter = iter->next) {
        iter->saved_crtc = drmModeGetCrtc(fd, iter->crtc);
        cout << "drmModeSetCrtc for fb id: " << iter->fb 
             << ", mode " << iter->mode.hdisplay << "x" << iter->mode.vdisplay 
             << ", saved_crtc... crtc_id: " << iter->saved_crtc->crtc_id << ", " 
             << iter->saved_crtc->width << "x" << iter->saved_crtc->height
             << endl;
        int ret = drmModeSetCrtc(fd, iter->crtc, iter->fb, 0, 0,
                        &iter->conn, 1, &iter->mode);
        if (ret) {
            fprintf(stderr, "cannot set CRTC for connector %u (%d): %m\n",
                iter->conn, errno);

            return -1;
        }
    }

    return 0;
}

void DrmDevice::cleanup(int fd)
{
    struct modeset_dev *iter;
    struct drm_mode_destroy_dumb dreq;

    while (modeset_list) {
        /* remove from global list */
        iter = modeset_list;
        modeset_list = iter->next;

	    cout << "Restoring original mode " << endl;

        /* restore saved CRTC configuration */
        drmModeSetCrtc(fd,
                    iter->saved_crtc->crtc_id,
                    iter->saved_crtc->buffer_id,
                    iter->saved_crtc->x,
                    iter->saved_crtc->y,
                    &iter->conn,
                    1,
                    &iter->saved_crtc->mode);
        drmModeFreeCrtc(iter->saved_crtc);
        munmap(iter->map, iter->size);

        /* delete framebuffer */
        drmModeRmFB(fd, iter->fb);

        /* delete dumb buffer */
        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = iter->handle;
        drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

        /* free allocated memory */
        free(iter);
    }

    if (ioctl(ttyFd, KDSETMODE, KD_TEXT) < 0) {
    	cerr << "Error restoring tty text mode" << endl;
    }

    // Stop being master
    //ioctl(fd, DRM_IOCTL_DROP_MASTER, 0);
}

// TODO: unused
bool DrmDevice::createFrameBuffer(int fd)
{
    return true;
}

void DrmDevice::swapBuffer()
{

}

// TODO: replace with int return value
bool DrmDevice::initialize()
{
    // Become the master of the DRI device
    // Commented as it errors
    //if (ioctl(fd, DRM_IOCTL_SET_MASTER, 0) < 0) {
    //	fprintf(stderr, "cannot become master (%d): %m\n", errno);
    //	return false;
    //}

    // Commented as it errors (probably not opening the right tty
    ttyFd = ::open("/dev/tty3", O_RDWR);
    if (ttyFd < 0) {
    	cerr << "Error opening tty" << endl;
    	return false;
    }

    if (ioctl(ttyFd, KDSETMODE, KD_GRAPHICS) < 0) {
    	cerr << "Error setting graphics on tty" << endl;
    	return false;
    }

    fd = open();
    if (fd < 0) {
        return false;
    }

    if (prepare(fd)) {
        ::close(fd);
        return false;
    }

    if (setMode(fd)) {
        ::close(fd);
        return false;
    }

    return true;
}

void DrmDevice::close()
{
    cleanup(fd);

    if (::close(fd)) {
        cerr << "Could not close dri file" << endl;
    }

    if (::close(ttyFd)) {
    	cerr << "Could not close tty file" << endl;
    }
}

void DrmDevice::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    assert(modeset_list);

    struct modeset_dev *iter = modeset_list;
    long bufferIndex = iter->bufferIndexForCoordinates(x, y);
    *(uint32_t *)&iter->map[bufferIndex] = iter->getPixelColor(r, g, b);
}

void DrmDevice::clearScreen()
{
    assert(modeset_list);

    struct modeset_dev *iter = modeset_list;
	memset(iter->map, 0, iter->size);
}

long DrmDevice::modeset_dev::bufferIndexForCoordinates(int x, int y) 
{
    return stride * y + x * 4;
}

uint32_t DrmDevice::modeset_dev::getPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) | (g << 8) | b; // FIXME: works only for 32bpp!
}
