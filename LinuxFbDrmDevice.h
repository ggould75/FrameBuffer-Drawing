#ifndef LINUXFBDRMDEVICE_H
#define LINUXFBDRMDEVICE_H

#include <sys/mman.h>

#include <vector>
#include <iostream> // TODO: remove

#include "Size.hpp"

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>

class Image;

class LinuxFbDrmDevice
{
public:
    LinuxFbDrmDevice(short int cardNumber);
    ~LinuxFbDrmDevice();

    bool open();
    void close();
    
    void createScreens();
    void createFramebuffers();
    void destroyFramebuffers();
    void setMode();
    void swapBuffers();

    int driFd() const;
    void setDriFd(int fd);
    
    static const short int kBufferCount = 2;
    
    struct Framebuffer {
        Framebuffer() : id(0), handle(0), pitch(0), length(0), data(MAP_FAILED), renderedImage(nullptr) { }
        uint32_t id;
        uint32_t handle;
        uint32_t pitch;
        size_t length;
        void *data;
        Image *renderedImage; // TODO: you sure pointer?
    };
    
    struct Output {
        Output() : connectorId(0), isModeSet(false), mode(0), drmFormat(DRM_FORMAT_XRGB8888), 
                   resolution(Size()), crtcId(0), oldCrtc(nullptr) { }
        ~Output() { 
            std::cout << "~Output()" << std::endl;
        }
        uint32_t connectorId;
        bool isModeSet;
        std::vector<drmModeModeInfo> modes;
        int mode;
        uint32_t drmFormat;
        Size resolution;
        uint32_t crtcId;
        drmModeCrtc *oldCrtc;
        Framebuffer fb[kBufferCount];
        
        void restoreMode(LinuxFbDrmDevice *device);
        void cleanup(LinuxFbDrmDevice *device);
    };
    
    int outputCount() const { return _mOutputs.size(); }
    Output *output(int index) { return &_mOutputs.at(index); }
    
private:    
    Output *createOutputForConnector(drmModeResPtr resources, drmModeConnectorPtr connector);
    int crtcForConnector(drmModeResPtr resources, drmModeConnectorPtr connector);
    bool createFramebuffer(Output *output, int bufferIndex);
    void destroyFramebuffer(Output *output, int bufferIndex);
    
    short int _mCardNumber;
    
    int _mDriFd;
    std::vector<Output> _mOutputs;
    uint32_t _mCrtcAllocator;
};

#endif // LINUXFBDRMDEVICE_H
