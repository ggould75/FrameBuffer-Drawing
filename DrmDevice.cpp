#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include <cassert>

#include "DrmDevice.hpp"

DrmDevice::DrmDevice(short int cardNumber) : cardNumber(cardNumber), list(new std::list<DrmDeviceSummary>) { }

DrmDevice::~DrmDevice()
{
    close();
    delete list;
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
    struct DrmDeviceSummary *dev;
    int ret;

    cardResources = drmModeGetResources(fd);
    if (!cardResources) {
        fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n",
            errno);
        
        return -errno;
    }

    for (i = 0; i < cardResources->count_connectors; ++i) {
        connector = drmModeGetConnector(fd, cardResources->connectors[i]);
        if (!connector) {
            fprintf(stderr, "cannot retrieve DRM connector %u:%u (%d): %m\n",
                i, cardResources->connectors[i], errno);
            
            continue;
        }

        dev = (DrmDeviceSummary *)malloc(sizeof(*dev));
        memset(dev, 0, sizeof(*dev));
        dev->conn = connector->connector_id;

        // TODO: rename setupConnector() ?
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

        drmModeFreeConnector(connector);
        list->push_front(*dev);
    }

    drmModeFreeResources(cardResources);

    return 0;
}

int DrmDevice::setupDevice(int fd, drmModeRes *cardResources, drmModeConnector *connector, struct DrmDeviceSummary *dev)
{
    int ret;

    if (connector->connection != DRM_MODE_CONNECTED) {
        fprintf(stderr, "ignoring unused connector %u\n",
            connector->connector_id);
        
        return -ENOENT;
    }

    // Make sure there's at least one valid mode
    if (connector->count_modes == 0) {
        fprintf(stderr, "no valid mode for connector %u\n",
            connector->connector_id);
        
        return -EFAULT;
    }
    
    // Copy mode information into local structure
    memcpy(&dev->mode, &connector->modes[0], sizeof(dev->mode));
    dev->width = connector->modes[0].hdisplay;
    dev->height = connector->modes[0].vdisplay;
    
    printf("mode for connector %u is %ux%u\n", connector->connector_id, dev->width, dev->height);

    // Look for CRTC for the connector
    ret = findCrtc(fd, cardResources, connector, dev);
    if (ret) {
        fprintf(stderr, "no valid crtc for connector %u\n",
            connector->connector_id);
        
        return ret;
    }

    // Finally create a framebuffer for the CRTC
    ret = createFB(fd, dev);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer for connector %u\n",
            connector->connector_id);
        
        return ret;
    }

    return 0;
}

int DrmDevice::findCrtc(int fd, drmModeRes *cardResources, drmModeConnector *connector, struct DrmDeviceSummary *dev)
{
    drmModeEncoder *enc;
    unsigned int i, j;
    int32_t crtc;
    struct modeset_dev *iter;

    // First try the currently connected encoder+crtc
    if (connector->encoder_id)
        enc = drmModeGetEncoder(fd, connector->encoder_id);
    else
        enc = NULL;

    if (enc) {
        if (enc->crtc_id) {
            crtc = enc->crtc_id;
            for (auto iter = list->begin(); iter != list->end(); ++iter) {
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
    for (i = 0; i < connector->count_encoders; ++i) {
        enc = drmModeGetEncoder(fd, connector->encoders[i]);
        if (!enc) {
            fprintf(stderr, "cannot retrieve encoder %u:%u (%d): %m\n",
                i, connector->encoders[i], errno);
            continue;
        }

        for (j = 0; j < cardResources->count_crtcs; ++j) {
            /* check whether this CRTC works with the encoder */
            if (!(enc->possible_crtcs & (1 << j)))
                continue;

            // Check that no other device already uses this CRTC
            crtc = cardResources->crtcs[j];
            for (auto iter = list->begin(); iter != list->end(); ++iter) {
                if (iter->crtc == crtc) {
                    crtc = -1;
                    break;
                }
            }

            // We have found a CRTC, so save it and return
            if (crtc >= 0) {
                drmModeFreeEncoder(enc);
                dev->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    fprintf(stderr, "cannot find suitable CRTC for connector %u\n",
        connector->connector_id);
    
    return -ENOENT;
}

int DrmDevice::createFB(int fd, struct DrmDeviceSummary *dev)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;
    int ret;

    // Create dumb buffer
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

    // Create framebuffer object for the dumb-buffer
    ret = drmModeAddFB(fd, dev->width, dev->height, 24, 32, dev->stride,
                dev->handle, &dev->fb);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_destroy;
    }

    // Prepare framebuffer for memory mapping */
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = dev->handle;
    ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        fprintf(stderr, "cannot map dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }

    dev->map = (uint8_t *)mmap(0, dev->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
    if (dev->map == MAP_FAILED) {
        fprintf(stderr, "cannot mmap dumb buffer (%d): %m\n",
            errno);
        ret = -errno;
        goto err_fb;
    }

    // Clear framebuffer (screen)
    memset(dev->map, 0, dev->size);

    return 0;

err_fb:
    drmModeRmFB(fd, dev->fb);
    
err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = dev->handle;
    drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
        
    return ret;
}

int DrmDevice::setMode(int fd)
{
    struct modeset_dev *iter;
    
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        iter->saved_crtc = drmModeGetCrtc(fd, iter->crtc);
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
    struct DrmDeviceSummary *iter;
    struct drm_mode_destroy_dumb dreq;

    while (!list->empty()) {
        iter = &list->front();
        
        // Restore saved CRTC configuration
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

        // Delete framebuffer
        drmModeRmFB(fd, iter->fb);

        // Delete dumb buffer
        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = iter->handle;
        drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

        list->pop_front();
    }
}

void DrmDevice::swapBuffer()
{
    // TODO
}

// TODO: replace with int return value
bool DrmDevice::initialize()
{
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
}

void DrmDevice::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    assert(list);
    
    struct DrmDeviceSummary iter = list->front();
    long bufferIndex = iter.bufferIndexForCoordinates(x, y);
    *(uint32_t *)&iter.map[bufferIndex] = iter.getPixelColor(r, g, b);
}

void DrmDevice::clearScreen()
{
    // TODO
}

long DrmDevice::DrmDeviceSummary::bufferIndexForCoordinates(int x, int y) 
{
    return stride * y + x * 4;
}

uint32_t DrmDevice::DrmDeviceSummary::getPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) | (g << 8) | b; // FIXME: works only for 32bpp!
}
