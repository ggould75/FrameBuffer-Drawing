#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cassert>
#include <iostream>

#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer(short int fbNum = 0) : fbNum(fbNum)
{

}

FrameBuffer::~FrameBuffer()
{
    if (m_frameBufferInfo == nullptr) {
        return;
    }

    if (m_frameBufferInfo->mappedMemory != nullptr) {
        munmap(m_frameBufferInfo->mappedMemory, m_frameBufferInfo->bufferSize());
    }

    delete m_frameBufferInfo;
}

void FrameBuffer::openDevice()
{
    char devicePath[16];
    sprintf(devicePath, "/dev/fb%d", fbNum);
    int fd = open(devicePath, O_RDWR);
    if (fd < 0) {
        cerr << "Error opening fb device! "
             << "Try launching with sudo or fb may not be enabled at kernel level." << endl;
        return;
    }

    m_frameBufferInfo = new FBInfo();
    m_frameBufferInfo->fd = fd;

    // Get variable and fixed screen info
    ioctl(fd, FBIOGET_VSCREENINFO, &m_frameBufferInfo->screenVarInfo);
    ioctl(fd, FBIOGET_FSCREENINFO, &m_frameBufferInfo->screenFixedInfo);

    // Map fb file to memory
    long bufferSize = m_frameBufferInfo->bufferSize();
    uint8_t *mappedMemory = (uint8_t *)mmap(0, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if (mappedMemory == MAP_FAILED) {
        cerr << "Couldn't map fb file to memory (errno: " << errno << ")" << endl;
        return;
    }
    
    m_frameBufferInfo->mappedMemory = mappedMemory;

    fb_var_screeninfo varInfo = m_frameBufferInfo->screenVarInfo;
    fb_fix_screeninfo fixedInfo = m_frameBufferInfo->screenFixedInfo;
    
    cout << "Visible res: " << varInfo.xres << " x " << varInfo.yres  
         << ", Virtual res: " << varInfo.xres_virtual << " x " << varInfo.yres_virtual << endl
         << "Framebuffer size: ~" << bufferSize / 1024 / 1024 << " MB"
         << ", Bytes/scanline: " << fixedInfo.line_length
         << ", grayscale: " << varInfo.grayscale
         << ", bpp: " << varInfo.bits_per_pixel << endl;
}

void FrameBuffer::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    assert(m_frameBufferInfo);
    assert(m_frameBufferInfo->mappedMemory);

    long bufferIndex = m_frameBufferInfo->bufferIndexForCoordinates(x, y);
    uint8_t *bufferStart = (uint8_t *)m_frameBufferInfo->mappedMemory;
    *((uint32_t *)(bufferStart + bufferIndex)) = pixelColorFromRGBComponents(r, g, b);
}

uint32_t FrameBuffer::pixelColorFromRGBComponents(uint8_t r, uint8_t g, uint8_t b)
{
    assert(m_frameBufferInfo);

    return m_frameBufferInfo->getPixelColor(r, g, b);
}

long FrameBuffer::FBInfo::bufferSize() {
    return screenVarInfo.yres_virtual * screenFixedInfo.line_length;
}

long FrameBuffer::FBInfo::bufferIndexForCoordinates(int x, int y) {
    return (x + screenVarInfo.xoffset) * (screenVarInfo.bits_per_pixel / 8) +
           (y + screenVarInfo.yoffset) * screenFixedInfo.line_length;
}

uint32_t FrameBuffer::FBInfo::getPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << screenVarInfo.red.offset) |
           (g << screenVarInfo.green.offset) |
           (b << screenVarInfo.blue.offset);
}
