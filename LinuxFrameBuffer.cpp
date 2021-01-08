#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cassert>
#include <iostream>

#include "LinuxFrameBuffer.hpp"
#include "common.h"

LinuxFrameBuffer::~LinuxFrameBuffer()
{
    if (m_frameBufferInfo == nullptr) {
        return;
    }

    if (m_frameBufferInfo->frontBuffer != nullptr) {
        close();
    }

    delete m_frameBufferInfo;
}

int LinuxFrameBuffer::open()
{
    char devicePath[16];
    
    sprintf(devicePath, "/dev/fb%d", fbNum);
    int fd = ::open(devicePath, O_RDWR);
    if (fd < 0) {
        cerr << "Error opening fb device! "
             << "Try launching with sudo or fb may not be enabled at kernel level." << endl;
        return -1;
    }
    
    return fd;
}

bool LinuxFrameBuffer::createFrameBuffer(int fd)
{
    m_frameBufferInfo = new FBInfo();
    m_frameBufferInfo->fd = fd;
    
    // Get variable and fixed screen info
    ioctl(fd, FBIOGET_VSCREENINFO, &m_frameBufferInfo->screenVarInfo);
    ioctl(fd, FBIOGET_FSCREENINFO, &m_frameBufferInfo->screenFixedInfo);

    // Map fb file to memory.
    // Note: enabling double buffer will cause mmap to fail on the majority of the graphics hardware (errno: 22)
    long bufferSize = m_frameBufferInfo->totalBufferSize();
    uint8_t *frontBuffer = (uint8_t *)mmap(0, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if (frontBuffer == MAP_FAILED) {
        cerr << "Couldn't map fb (errno: " << errno << ")" << endl;
        ::close(fd);
        return false;
    }
    
    m_frameBufferInfo->frontBuffer = frontBuffer;
#if ENABLE_DOUBLE_BUFFER == 1
    m_frameBufferInfo->backBuffer = frontBuffer + m_frameBufferInfo->screenBufferSize();
#else
    m_frameBufferInfo->backBuffer = frontBuffer;
#endif
    
    fb_var_screeninfo varInfo = m_frameBufferInfo->screenVarInfo;
    fb_fix_screeninfo fixedInfo = m_frameBufferInfo->screenFixedInfo;
    
    ::close(fd);
    
#if ENABLE_DEBUG == 1
    cout << "Visible res: " << varInfo.xres << " x " << varInfo.yres  
         << ", Virtual res: " << varInfo.xres_virtual << " x " << varInfo.yres_virtual
         << ", Offset: (" << varInfo.xoffset << ", " << varInfo.yoffset << ")" << endl
         << "Framebuffer size: ~" << bufferSize / 1024 / 1024 << " MB"
         << ", Bytes/scanline: " << fixedInfo.line_length
         << ", grayscale: " << varInfo.grayscale
         << ", bpp: " << varInfo.bits_per_pixel << endl
         << "Acceleration type: " << fixedInfo.accel << endl;
#endif
         
    return true;
}

void LinuxFrameBuffer::swapBuffer()
{
    fb_var_screeninfo varInfo = m_frameBufferInfo->screenVarInfo;
    if (varInfo.yoffset == 0) {
        varInfo.yoffset = m_frameBufferInfo->screenBufferSize();
    } else {
        varInfo.yoffset = 0;
    }
    
    // Pan to the back buffer
    ioctl(m_frameBufferInfo->fd, FBIOPAN_DISPLAY, &varInfo);

    void *tmpBuffer = m_frameBufferInfo->frontBuffer;
    m_frameBufferInfo->frontBuffer = m_frameBufferInfo->backBuffer;
    m_frameBufferInfo->backBuffer = tmpBuffer;
}

bool LinuxFrameBuffer::initialize()
{
    int fd = open();
    if (fd > 0) {
        return createFrameBuffer(fd);
    }
    
    return false;
}

void LinuxFrameBuffer::close()
{
    assert(m_frameBufferInfo);
    assert(m_frameBufferInfo->frontBuffer);
    
    munmap(m_frameBufferInfo->frontBuffer, m_frameBufferInfo->totalBufferSize());
}

void LinuxFrameBuffer::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    assert(m_frameBufferInfo);
    assert(m_frameBufferInfo->frontBuffer);

    long bufferIndex = m_frameBufferInfo->bufferIndexForCoordinates(x, y);
    uint8_t *bufferStart = (uint8_t *)m_frameBufferInfo->backBuffer;
    *((uint32_t *)(bufferStart + bufferIndex)) = pixelColorFromRGBComponents(r, g, b);
}

uint32_t LinuxFrameBuffer::pixelColorFromRGBComponents(uint8_t r, uint8_t g, uint8_t b)
{
    assert(m_frameBufferInfo);

    return m_frameBufferInfo->getPixelColor(r, g, b);
}

void LinuxFrameBuffer::clearScreen()
{
    assert(m_frameBufferInfo);
    assert(m_frameBufferInfo->frontBuffer);
    
    void *bufferStart = m_frameBufferInfo->backBuffer;
    memset(bufferStart, 0, m_frameBufferInfo->screenBufferSize());
}

long LinuxFrameBuffer::FBInfo::screenBufferSize() 
{
    return screenVarInfo.yres_virtual * screenFixedInfo.line_length;
}

long LinuxFrameBuffer::FBInfo::totalBufferSize() 
{
#if ENABLE_DOUBLE_BUFFER == 1
    return screenBufferSize() * 2;
#else
    return screenBufferSize();
#endif
}

long LinuxFrameBuffer::FBInfo::bufferIndexForCoordinates(int x, int y) 
{
    return (x + screenVarInfo.xoffset) * (screenVarInfo.bits_per_pixel / 8) +
           (y + screenVarInfo.yoffset) * screenFixedInfo.line_length;
}

uint32_t LinuxFrameBuffer::FBInfo::getPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << screenVarInfo.red.offset) |
           (g << screenVarInfo.green.offset) |
           (b << screenVarInfo.blue.offset);
}
