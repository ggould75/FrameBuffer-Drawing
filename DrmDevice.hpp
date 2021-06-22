#ifndef DRMDEVICE_H
#define DRMDEVICE_H

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "Device.hpp"

class DrmDevice : public Device
{
public:
    DrmDevice(short int cardNumber) : cardNumber(cardNumber) { }
    ~DrmDevice();

    bool initialize() override;
    
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) override;
    void clearScreen() override;
    
protected:
    int open() override;
    void close() override;
    bool createFrameBuffer(int fd) override;
    void swapBuffer() override;
    
private:
    struct modeset_dev {
        struct modeset_dev *next;

        uint32_t width;
        uint32_t height;
        uint32_t stride;
        uint64_t size;
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
    int setupDevice(int fd, drmModeRes *res, drmModeConnector *conn, modeset_dev *dev);
    int findCrtc(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev);
    int createFB(int fd, struct modeset_dev *dev);
    int setMode(int fd);
    void cleanup(int fd);
    
    struct modeset_dev *modeset_list {NULL};
    short int cardNumber;
    int fd {-1};
	int ttyFd {-1};
};

#endif // DRMDEVICE_H
