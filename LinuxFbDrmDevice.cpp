#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include <iostream>
#include <cassert>

#include "LinuxFbDrmDevice.h"
#include "Image.hpp"

using namespace std;

LinuxFbDrmDevice::LinuxFbDrmDevice(short int cardNumber) 
    : _mCardNumber(cardNumber), 
      _mCrtcAllocator(0) { }

LinuxFbDrmDevice::~LinuxFbDrmDevice()
{
    close();
}

bool LinuxFbDrmDevice::open()
{
    char devicePath[16];
    
    sprintf(devicePath, "/dev/dri/card%d", _mCardNumber);
    int fd = ::open(devicePath, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        cerr << "Error opening " << devicePath << endl;
        return false;
    }
    
    uint64_t hasDumbBuffers;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &hasDumbBuffers) < 0 || !hasDumbBuffers) {
        cerr << "DRM device " << devicePath << " does not support dumb buffers" << endl;
        ::close(fd);
        return false;
    }
    
    setDriFd(fd);
    
    return true;
}

void LinuxFbDrmDevice::close()
{
    for (Output &output : _mOutputs) {
        output.cleanup(this);
    }
    
    _mOutputs.clear();
    
    if (_mDriFd > 0) {
        ::close(_mDriFd);
        setDriFd(-1);
    }
}

void LinuxFbDrmDevice::createScreens()
{
    assert(_mDriFd);

    drmModeResPtr resources = drmModeGetResources(_mDriFd);
    if (!resources) {
        fprintf(stderr, "Cannot retrieve drm card resources (%d): %s\n", errno, strerror(errno));
        ::close(_mDriFd);
        return;
    }

    for (unsigned int i = 0; i < resources->count_connectors; ++i) {
        drmModeConnectorPtr connector = drmModeGetConnector(_mDriFd, resources->connectors[i]);
        if (!connector) {
            fprintf(stderr, "Cannot retrieve drm connector %u (%d): %s\n", i, errno, strerror(errno));
            continue;
        }

        Output *output = createOutputForConnector(resources, connector);
        _mOutputs.push_back(*output);
        // TODO: check: if push_back creates a copy you are leaking output
        
        drmModeFreeConnector(connector);
    }

    drmModeFreeResources(resources);
}

void LinuxFbDrmDevice::createFramebuffers()
{
    for (Output &output : _mOutputs) {
        for (int i = 0; i < kBufferCount; ++i) {
            if (!createFramebuffer(&output, i)) {
                return;
            }
        }
        //output.backFb = 0;
        //output.flipped = false;
    }
}

bool LinuxFbDrmDevice::createFramebuffer(LinuxFbDrmDevice::Output *output, int bufferIndex)
{
    const uint32_t width = output->resolution.width();
    const uint32_t height = output->resolution.height();
    const uint32_t bpp = 32; // FIXME: take it from output
    
    drm_mode_create_dumb creq;
    memset(&creq, 0, sizeof(creq));
    creq.width = width;
    creq.height = height;
    creq.bpp = bpp;
    if (drmIoctl(_mDriFd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) < 0) {
        cerr << "Could not create dumb buffer (" << errno << "): " << strerror(errno) << endl;
        return false;
    }
    
    Framebuffer &fb(output->fb[bufferIndex]);
    fb.handle = creq.handle;
    fb.pitch = creq.pitch;
    fb.length = creq.size;
    
    uint32_t handles[4] = { fb.handle };
    uint32_t strides[4] = { fb.pitch };
    uint32_t offsets[4] = { 0 };
    
    if (drmModeAddFB2(_mDriFd, width, height, 32, handles, strides, offsets, &fb.id, 0)) { // FIXME: take pixel format from output
        cerr << "Failed to add (" << errno << "): " << strerror(errno) << endl;
        return false;
    }
    
    drm_mode_map_dumb mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = fb.handle;
    if (drmIoctl(_mDriFd, DRM_IOCTL_MODE_MAP_DUMB, &mreq) < 0) {
        cerr << "Failed to map dumb buffer (" << errno << "): " << strerror(errno) << endl;
        return false;
    }

    fb.data = (uint8_t *)mmap(0, fb.length, PROT_READ | PROT_WRITE, MAP_SHARED, _mDriFd, mreq.offset);
    if (fb.data == MAP_FAILED) {
        cerr << "Failed to mmap dumb buffer (" << errno << "): " << strerror(errno) << endl;
        return false;
    }

    memset(fb.data, 0, fb.length);
    
    fb.renderedImage = new Image(); // TODO
    
    fprintf(stdout, "Framebuffer %u, pixel format: 0x%x, mapped at %p", fb.id, 32, fb.data);
    
    return true;
}

void LinuxFbDrmDevice::destroyFramebuffers()
{
    for (Output &output : _mOutputs) {
        for (int i = 0; i < kBufferCount; ++i) {
            destroyFramebuffer(&output, i);
        }
    }
}

void LinuxFbDrmDevice::destroyFramebuffer(LinuxFbDrmDevice::Output *output, int bufferIndex)
{
    Framebuffer &fb(output->fb[bufferIndex]);
    
    if (fb.data != MAP_FAILED) {
        munmap(fb.data, fb.length);
    }
    if (fb.id) {
        if (drmModeRmFB(_mDriFd, fb.id) < 0) {
            cerr << "Failed to remove fb (" << errno << "): " << strerror(errno) << endl;
        }
    }
    if (fb.handle) {
        drm_mode_destroy_dumb dreq = { fb.handle };
        if (drmIoctl(_mDriFd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq) == -1) {
            cerr << "Failed to destroy dumb buffer " << fb.handle << endl;
        }
    }
    
    fb = Framebuffer();
}

void LinuxFbDrmDevice::setMode()
{
    for (Output &output : _mOutputs) {
        drmModeModeInfo &modeInfo(output.modes[output.mode]);
        if (drmModeSetCrtc(_mDriFd, output.crtcId, output.fb[0].id, 0, 0,
                            &output.connectorId, 1, &modeInfo) == -1) {
            cerr << "Failed to set mode" << endl;
            return;
        }
        
        output.isModeSet = true;
    }
}

void LinuxFbDrmDevice::swapBuffers()
{
    // TODO
}

int LinuxFbDrmDevice::driFd() const
{
    return _mDriFd;
}

void LinuxFbDrmDevice::setDriFd(int fd)
{
    _mDriFd = fd;
}

LinuxFbDrmDevice::Output *LinuxFbDrmDevice::createOutputForConnector(drmModeResPtr resources, 
                                                                     drmModeConnectorPtr connector)
{
    if (connector->connection != DRM_MODE_CONNECTED) {
        cerr << "Ignoring unused connector " << connector->connector_id << endl;
        return nullptr;
    }

    if (connector->count_modes == 0) {
        cerr << "No valid mode for connector " << connector->connector_id << endl;        
        return nullptr;
    }
    
    const int crtc = crtcForConnector(resources, connector);
    if (crtc < 0) {
        cerr << "No usable crtc/encoder pair for the connector" << endl;
        return nullptr;
    }
    
    const uint32_t crtcId = resources->crtcs[crtc];
    
    vector<drmModeModeInfo> modes;
    modes.reserve(connector->count_modes);
    cout << "Connector has " << connector->count_modes << " modes, crtc index: " << crtc 
         << ", crtcId: " << crtcId << endl;
    for (int i = 0; i < connector->count_modes; i++) {
        const drmModeModeInfo &mode = connector->modes[i];
        cout << "mode " << i << ": " << mode.hdisplay << "x" << mode.vdisplay 
             << '@' << mode.vrefresh << "hz";
        modes.push_back(connector->modes[i]);
    }
    
    int selectedModeIndex = 0;
    drmModeModeInfo selectedMode = modes.at(selectedModeIndex);
    int width = selectedMode.hdisplay;
    int height = selectedMode.vdisplay;
    int refreshFreq = selectedMode.vrefresh;
    cout << "Using mode " << selectedModeIndex << ": " << width << "x" << height
             << '@' << refreshFreq << " hz" << endl;
    
    Output *output = new Output();
    output->connectorId = connector->connector_id;
    output->crtcId = crtcId;
    output->modes = modes;
    output->oldCrtc = drmModeGetCrtc(_mDriFd, crtcId);
    output->resolution = Size(width, height);
    
    _mCrtcAllocator |= (1 << crtc);
    
    return output;
}

int LinuxFbDrmDevice::crtcForConnector(drmModeResPtr resources, drmModeConnectorPtr connector)
{
    int candidate = -1;

    for (int i = 0; i < connector->count_encoders; i++) {
        drmModeEncoderPtr encoder = drmModeGetEncoder(_mDriFd, connector->encoders[i]);
        if (!encoder) {
            fprintf(stderr, "Cannot retrieve encoder %u:%u (%d): %s\n", i, connector->encoders[i], errno, 
                    strerror(errno));
            continue;
        }

        uint32_t encoderId = encoder->encoder_id;
        uint32_t crtcId = encoder->crtc_id;
        uint32_t possibleCrtcs = encoder->possible_crtcs;
        
        drmModeFreeEncoder(encoder);

        for (int j = 0; j < resources->count_crtcs; j++) {
            bool isPossible = possibleCrtcs & (1 << j);
            bool isAvailable = !(_mCrtcAllocator & (1 << j));
            // Preserve the existing CRTC -> encoder -> connector routing if
            // any. It makes the initialization faster, and may be better
            // since we have a very dumb picking algorithm.
            bool isBestChoice = (!connector->encoder_id ||
                                    (connector->encoder_id == encoderId &&
                                    resources->crtcs[j] == crtcId));

            if (isPossible && isAvailable && isBestChoice) {
                return j;
            } else if (isPossible && isAvailable) {
                candidate = j;
            }
        }
    }

    return candidate;
}

void LinuxFbDrmDevice::Output::restoreMode(LinuxFbDrmDevice *device)
{
    if (isModeSet && oldCrtc) {
        drmModeSetCrtc(device->driFd(), 
                       oldCrtc->crtc_id, 
                       oldCrtc->buffer_id, 
                       0, 0, 
                       &connectorId, 1, 
                       &oldCrtc->mode);
        isModeSet = false;
    }
}

void LinuxFbDrmDevice::Output::cleanup(LinuxFbDrmDevice *device)
{
    restoreMode(device);
    
    if (oldCrtc) {
        drmModeFreeCrtc(oldCrtc);
        oldCrtc = nullptr;
    }
}
