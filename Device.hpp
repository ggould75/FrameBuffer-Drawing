//
//  Device.hpp
//  LowLevelDrawing
//
//  Created by Marco Mussini on 22/12/2020.
//

#ifndef Device_hpp
#define Device_hpp

#include <stdio.h>
#include <fcntl.h>

#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cassert>
#include <iostream>

using namespace std;

class Device
{
public:
    virtual ~Device() = default;
    virtual void openDevice() = 0;
    virtual void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual uint32_t pixelColorFromRGBComponents(uint8_t r, uint8_t g, uint8_t b) = 0;
};

class FrameBuffer : public Device
{
public:
    FrameBuffer(short int fbNum = 0) : fbNum(fbNum) {}

    ~FrameBuffer()
    {
        if (m_frameBufferInfo == nullptr) {
            return;
        }

        if (m_frameBufferInfo->memoryPtr != nullptr) {
            munmap(m_frameBufferInfo->memoryPtr, m_frameBufferInfo->bufferSize());
        }

        delete m_frameBufferInfo;
    }

    void openDevice()
    {
        char devicePath[16];
        sprintf(devicePath, "/dev/fb%d", fbNum);
        int fd = open(devicePath, O_RDWR);
        if (fd < 0) {
            cerr << "Error opening fb device! "
                 << "Try launching with sudo or fb may not be enabled at kernel level." << endl;
            return;
        }

        m_frameBufferInfo = new fb_info();
        m_frameBufferInfo->fd = fd;

        // Get variable and fixed screen info
        ioctl(fd, FBIOGET_VSCREENINFO, &m_frameBufferInfo->screenVarInfo);
        ioctl(fd, FBIOGET_FSCREENINFO, &m_frameBufferInfo->screenFixedInfo);

        // Map fb file to memory
        long bufferSize = m_frameBufferInfo->bufferSize();
        uint8_t *memoryPtr =
            (uint8_t *)mmap(0, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
        if (memoryPtr == MAP_FAILED) {
            cerr << "Couldn't map fb file to memory (errno: " << errno << ")" << endl;
            return;
        }
        m_frameBufferInfo->memoryPtr = memoryPtr;

        printf("%d x %d, Framebuffer size: ~%ld MB, Bytes/scanline: %d, grayscale: %d, bpp: %d\n",
    		m_frameBufferInfo->screenVarInfo.xres_virtual, m_frameBufferInfo->screenVarInfo.yres_virtual,
            bufferSize / 1024 / 1024, m_frameBufferInfo->screenFixedInfo.line_length,
            m_frameBufferInfo->screenVarInfo.grayscale, m_frameBufferInfo->screenVarInfo.bits_per_pixel);
    }

    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
    {
        assert(m_frameBufferInfo);
        assert(m_frameBufferInfo->memoryPtr);

        long bufferIndex = m_frameBufferInfo->bufferIndexForCoordinates(x, y);
        uint8_t *bufferStart = (uint8_t *)m_frameBufferInfo->memoryPtr;
        *((uint32_t *)(bufferStart + bufferIndex)) = pixelColorFromRGBComponents(0x00, 0xDD, 0x22);
    }

    uint32_t pixelColorFromRGBComponents(uint8_t r, uint8_t g, uint8_t b)
    {
        assert(m_frameBufferInfo);

        return m_frameBufferInfo->getPixelColor(r, g, b);
    }

private:
    struct fb_info
    {
        int fd {-1};
        void *memoryPtr {nullptr};
        struct fb_var_screeninfo screenVarInfo;
        struct fb_fix_screeninfo screenFixedInfo;
        unsigned bytesPerPixel;

        long bufferSize() {
            return screenVarInfo.yres_virtual * screenFixedInfo.line_length;
        }

        long bufferIndexForCoordinates(int x, int y) {
            return (x + screenVarInfo.xoffset) * (screenVarInfo.bits_per_pixel / 8) +
                   (y + screenVarInfo.yoffset) * screenFixedInfo.line_length;
        }

        uint32_t getPixelColor(uint8_t r, uint8_t g, uint8_t b)
        {
        	return (r << screenVarInfo.red.offset) |
                   (g << screenVarInfo.green.offset) |
                   (b << screenVarInfo.blue.offset);
        }
    };

    short int fbNum;
    fb_info *m_frameBufferInfo {nullptr};
};

class GraphicContext
{
public:
    GraphicContext()
    {
        device = new FrameBuffer();
        device->openDevice();
    }

    void drawPixel(int x, int y)
    {
        device->drawPixel(x, y, 0xFF, 0x00, 0x00);
    }

    void drawLine(int x1, int y1, int x2, int y2)
    {
        // TODO: use device->drawPixel(x, y) to draw all pixels...
    }

private:
    Device *device;
};

#endif /* Device_hpp */
