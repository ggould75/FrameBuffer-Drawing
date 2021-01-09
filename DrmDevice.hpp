#ifndef DRMDEVICE_H
#define DRMDEVICE_H

#include <xf86drm.h>
#include <xf86drmMode.h>

#include <list>

#include "Device.hpp"

class DrmDevice : public Device
{
public:
    DrmDevice(short int cardNumber);
    ~DrmDevice();

    bool initialize() override;
    
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) override;
    void clearScreen() override;
    
protected:
    int open() override;
    void close() override;
    void swapBuffer() override;
    
private:
    struct DrmDeviceSummary {
        uint32_t width;
        uint32_t height;
        uint32_t stride;
        uint32_t size;
        uint32_t handle;
        uint8_t *map {NULL};

        drmModeModeInfo mode;
        uint32_t fb;
        uint32_t conn;
        uint32_t crtc;
        drmModeCrtc *saved_crtc;
        
        long bufferIndexForCoordinates(int x, int y);
        uint32_t getPixelColor(uint8_t r, uint8_t g, uint8_t b);
    };
    
    int prepare(int fd);
    int setupDevice(int fd, drmModeRes *cardResources, drmModeConnector *connector, DrmDeviceSummary *dev);
    int findCrtc(int fd, drmModeRes *cardResources, drmModeConnector *connector, struct DrmDeviceSummary *dev);
    int createFB(int fd, struct DrmDeviceSummary *dev);
    int setMode(int fd);
    void cleanup(int fd);
    
    std::list<DrmDeviceSummary> *list {NULL};
    short int cardNumber;
    int fd {-1};
};

#endif // DRMDEVICE_H
