#ifndef LINUXFRAMEBUFFER_H
#define LINUXFRAMEBUFFER_H

#include <linux/fb.h>

#include "Device.hpp"

class LinuxFrameBuffer : public Device
{
public:
    LinuxFrameBuffer(short int fbNum) : fbNum(fbNum) { }
    ~LinuxFrameBuffer();
    
    bool initialize() override;
    
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) override;
    void clearScreen() override;
    
    uint32_t pixelColorFromRGBComponents(uint8_t r, uint8_t g, uint8_t b);

protected:
    int open() override;
    void close() override;
    bool createFrameBuffer(int fd) override;
    void swapBuffer() override;
    
private:
    struct FBInfo
    {
        int fd {-1};
        void *frontBuffer {nullptr};
        void *backBuffer {nullptr};
        struct fb_var_screeninfo screenVarInfo;
        struct fb_fix_screeninfo screenFixedInfo;
        unsigned bytesPerPixel;

        long screenBufferSize();
        long totalBufferSize();
        long bufferIndexForCoordinates(int x, int y);
        uint32_t getPixelColor(uint8_t r, uint8_t g, uint8_t b);
    };

    short int fbNum;
    FBInfo *m_frameBufferInfo {nullptr};
};

#endif // LINUXFRAMEBUFFER_H
