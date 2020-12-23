#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <linux/fb.h>

#include "Device.hpp"

class FrameBuffer : public Device
{
public:
    FrameBuffer(short int fbNum);
    ~FrameBuffer();
    

    void openDevice();
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    uint32_t pixelColorFromRGBComponents(uint8_t r, uint8_t g, uint8_t b);

private:
    struct FBInfo
    {
        int fd {-1};
        void *mappedMemory {nullptr};
        struct fb_var_screeninfo screenVarInfo;
        struct fb_fix_screeninfo screenFixedInfo;
        unsigned bytesPerPixel;

        long bufferSize();
        long bufferIndexForCoordinates(int x, int y);
        uint32_t getPixelColor(uint8_t r, uint8_t g, uint8_t b);
    };

    short int fbNum;
    FBInfo *m_frameBufferInfo {nullptr};
};

#endif // FRAMEBUFFER_H
