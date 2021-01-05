#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cassert>
#include <iostream>

#include "FrameBuffer.hpp"

FrameBuffer::~FrameBuffer()
{
    if (m_frameBufferInfo == nullptr) {
        return;
    }

    if (m_frameBufferInfo->mappedMemory != nullptr) {
        close();
    }

    delete m_frameBufferInfo;
}

int FrameBuffer::open()
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

bool FrameBuffer::createFrameBuffer(int fd)
{
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
        ::close(fd);
        return false;
    }
    
    m_frameBufferInfo->mappedMemory = mappedMemory;

    fb_var_screeninfo varInfo = m_frameBufferInfo->screenVarInfo;
    fb_fix_screeninfo fixedInfo = m_frameBufferInfo->screenFixedInfo;
    
    ::close(fd);
    
#ifdef ENABLE_DEBUG
    cout << "Visible res: " << varInfo.xres << " x " << varInfo.yres  
         << ", Virtual res: " << varInfo.xres_virtual << " x " << varInfo.yres_virtual
         << ", Offset: (" << varInfo.xoffset << ", " << varInfo.yoffset << ")" << endl
         << "Framebuffer size: ~" << bufferSize / 1024 / 1024 << " MB"
         << ", Bytes/scanline: " << fixedInfo.line_length
         << ", grayscale: " << varInfo.grayscale
         << ", bpp: " << varInfo.bits_per_pixel << endl;
#endif
         
    return true;
}

bool FrameBuffer::initialize()
{
    int fd = open();
    if (fd > 0) {
        return createFrameBuffer(fd);
    }
    
    return false;
}

void FrameBuffer::close()
{
    assert(m_frameBufferInfo);
    assert(m_frameBufferInfo->mappedMemory);
    
    munmap(m_frameBufferInfo->mappedMemory, m_frameBufferInfo->bufferSize());
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

void FrameBuffer::clearScreen()
{
    assert(m_frameBufferInfo);
    assert(m_frameBufferInfo->mappedMemory);
    
    void *bufferStart = m_frameBufferInfo->mappedMemory;
    memset(bufferStart, 0, m_frameBufferInfo->bufferSize());
    
#ifdef ENABLE_DEBUG
    cout.flags(ios::hex | ios::showbase);
    cout <<  bufferStart << dec << ", size: " << m_frameBufferInfo->bufferSize() / 1024 / 1024 << endl;
#endif
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
