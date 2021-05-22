#include "LinuxFbScreen.h"
#include "Painter.hpp"
#include "Rect.hpp"
#include "common.h"

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <iostream>

#include <linux/fb.h>
#include <linux/kd.h>

using namespace std;

// Try to adjust screen depth from color components lengths.
// Sometime these are not available and depth has to be reverted to its original value.
static int adjustDepthIfNeeded(const fb_var_screeninfo &variableInfo)
{
    int depth = variableInfo.bits_per_pixel;
    if (depth == 24 || depth == 16) {
        int adjustedDepth = variableInfo.red.length + variableInfo.green.length + variableInfo.blue.length;
        if (adjustedDepth > 0) {
            depth = adjustedDepth;
        }
    }
    
    return depth;
}

static Rect adjustGeometry(const fb_var_screeninfo &variableInfo)
{
    int top = variableInfo.xoffset;
    int left = variableInfo.yoffset;
    int width = variableInfo.xres;
    int height = variableInfo.yres;
    
    if (width == 0 || height == 0) {
        cerr << "Screen geometry undefined, using 320x240" << endl;
        width = 320;
        height = 240;
    }
    
    return Rect(left, top, width, height);
}

static Image::Format determineFormat(const fb_var_screeninfo &info, int depth)
{
    const fb_bitfield rgba[4] = {
        info.red,
        info.green,
        info.blue,
        info.transp
    };
    
    Image::Format format = Image::Format_Invalid;
    switch (depth) {
        case 16: {
            // For little-endian systems
            const fb_bitfield rgb565[4] = {{11, 5, 0}, {5, 6, 0},
                                           {0, 5, 0}, {0, 0, 0}};
            // For big-endian systems                               
            const fb_bitfield bgr565[4] = {{0, 5, 0}, {5, 6, 0},
                                           {11, 5, 0}, {0, 0, 0}};
            if (memcmp(rgba, rgb565, 3 * sizeof(fb_bitfield)) == 0) {
                format = Image::Format_RGB16;
            } else if (memcmp(rgba, bgr565, 3 * sizeof(fb_bitfield)) == 0) {
                format = Image::Format_RGB16;
            }
            break;
        }
        default:
            break;
    }
    
    return format;
}

static int openTtyDevice()
{
    const char *const ttyDevs[] = {
        "/dev/tty",
        "/dev/tty0",
        "/dev/ttyS0",
        "/dev/console", 0
    };
    
    int fd = -1;
    for (const char * const *dev = ttyDevs; *dev; ++dev) {
        if ((fd = open(*dev, O_RDWR | O_CLOEXEC)) < 0) {
            cerr << "Failed to open " << *dev << " (" << errno << "): " << strerror(errno) << endl;
        } else {
            cout << "Valid " << *dev << " found" << endl;
            break;
        }
    }
    
    return fd;
}

static void restoreAndCloseTty(int ttyFd, int oldMode)
{
    ioctl(ttyFd, KDSETMODE, oldMode);
    close(ttyFd);
}

static void switchToGraphicsMode(int ttyFd, int *oldMode)
{
    if (ioctl(ttyFd, KDGETMODE, oldMode) < 0) {
        cerr << "KDGETMODE failed (" << errno << "): " << strerror(errno) << endl;
        return;
    }
    if (*oldMode != KD_GRAPHICS) {
        ioctl(ttyFd, KDSETMODE, KD_GRAPHICS);
    }
}

static void blankScreen(int ttyFd)
{
    int result = ioctl(ttyFd, FBIOBLANK, VESA_NO_BLANKING);
    if (result < 0) {
        cerr << "FBIOBLANK failed (" << errno << "): " << strerror(errno) << endl;
        return;
    }
}

LinuxFbScreen::LinuxFbScreen(short int fbNumber) 
    : _mFbNumber(fbNumber),
      _mFbFd(-1), 
      _mTtyFd(-1),
      _mOldTtyMode(-1),
      _shouldSaveAndRestoreTtyMode(false),
      _mBlitterPainter(nullptr)
{
    _mMmap.data = nullptr;
}

LinuxFbScreen::~LinuxFbScreen()
{
    if (_mFbFd != -1) {
        if (_mMmap.data) {
            munmap(_mMmap.data - _mMmap.offset, _mMmap.length);
        }
        close(_mFbFd);
    }
    
    if (_mTtyFd != -1) {
        restoreAndCloseTty(_mTtyFd, _mOldTtyMode);
    }
    
    delete _mBlitterPainter;
}

bool LinuxFbScreen::initialize() 
{
    char devicePath[12];
    sprintf(devicePath, "/dev/fb%d", _mFbNumber);
    
    int openFlags = O_RDWR | O_CLOEXEC;
    
    struct stat sb;
    
    if (stat(devicePath, &sb) < 0) {
        cerr << "Couldn't find " << devicePath << ". Trying with alternative path..." << endl;
        
        char deviceAltPath[20];
        sprintf(deviceAltPath, "/dev/graphics/fb%d", _mFbNumber);
        if (stat(deviceAltPath, &sb) < 0) {
            cerr << "Couldn't find " << deviceAltPath << endl;
            return false;
        }
        
        if ((_mFbFd = open(deviceAltPath, openFlags)) < 0) {
            cerr << "Failed to open " << deviceAltPath << " (" << errno << "): " << strerror(errno) << endl;
            return false;
        }
    } else {
        if ((_mFbFd = open(devicePath, openFlags)) < 0) {
            cerr << "Failed to open " << devicePath << " (" << errno << "): " << strerror(errno) << endl;
            return false;
        }
    }
        
    fb_fix_screeninfo fixedInfo;
    fb_var_screeninfo variableInfo;
    memset(&fixedInfo, 0, sizeof(fixedInfo));
    memset(&variableInfo, 0, sizeof(variableInfo));
    
    if (ioctl(_mFbFd, FBIOGET_FSCREENINFO, &fixedInfo) < 0) {
        cerr << "Unable to read fb fixed info " << " (" << errno << "): " << strerror(errno) << endl;
        return false;
    }
    if (ioctl(_mFbFd, FBIOGET_VSCREENINFO, &variableInfo) < 0) {
        cerr << "Unable to read fb variable info " << " (" << errno << "): " << strerror(errno) << endl;
        return false;
    }
    
    mDepth = adjustDepthIfNeeded(variableInfo);
    Rect absoluteGeometry = adjustGeometry(variableInfo);
    Rect relativeGeometry = Rect(absoluteGeometry.size());
    mGeometry = relativeGeometry;
    mFormat = determineFormat(variableInfo, mDepth);
    
    unsigned char *data = (unsigned char *)mmap(0, fixedInfo.smem_len, 
                                                PROT_READ | PROT_WRITE, MAP_SHARED, _mFbFd, 0);
    if (data == MAP_FAILED) {
        cerr << "Failed to map fb (" << errno << "): " << strerror(errno) << endl;
        return false;
    }
    
    _mMmap.bytesPerLine = fixedInfo.line_length;
    _mMmap.length = fixedInfo.smem_len;
    _mMmap.offset = absoluteGeometry.top() * _mMmap.bytesPerLine + absoluteGeometry.left() * mDepth / 8;
    _mMmap.data = data + _mMmap.offset;
    
    FbScreen::initializeCompositor();
    _mFbScreenImage = 
        Image(_mMmap.data, absoluteGeometry.width(), absoluteGeometry.height(), _mMmap.bytesPerLine, mFormat);
    
    if (_shouldSaveAndRestoreTtyMode && (_mTtyFd = openTtyDevice()) > 0) {
        switchToGraphicsMode(_mTtyFd, &_mOldTtyMode);
        blankScreen(_mTtyFd);
    }
    
#if ENABLE_DEBUG == 1
    cout << "Visible res: " << variableInfo.xres << " x " << variableInfo.yres  
         << ", Virtual res: " << variableInfo.xres_virtual << " x " << variableInfo.yres_virtual
         << ", Offset: (" << variableInfo.xoffset << ", " << variableInfo.yoffset << ")" << endl
         << "Framebuffer size: ~" << fixedInfo.smem_len / 1024 / 1024 << " MB"
         << ", Bytes/scanline: " << fixedInfo.line_length
         << ", grayscale: " << variableInfo.grayscale
         << ", bpp: " << variableInfo.bits_per_pixel << endl
         << "Acceleration type: " << fixedInfo.accel << endl;
#endif
         
    return true;
}

void LinuxFbScreen::clearScreen()
{
    memset(_mMmap.data, 0, _mMmap.length);
}

void LinuxFbScreen::redraw()
{
    // TODO
    // Invoke redraw() on parent
    // Create Painter on _mFbScreenImage
    // Foreach touched region returned by parent
    //    draw the corresponding portion of mScreenImage
    //
    // Shall I return a region or rect as the parent?
}

void LinuxFbScreen::redraw(Image *image)
{
    FbScreen::redraw(image);
    
    if (!_mBlitterPainter) {
        _mBlitterPainter = new Painter(&_mFbScreenImage);
    }
    
//    if (touched.isEmpty())
//        return touched;
//
//    if (!mBlitter)
//        mBlitter = new QPainter(&mFbScreenImage);
//
//    mBlitter->setCompositionMode(QPainter::CompositionMode_Source);
//    for (const QRect &rect : touched)
//        mBlitter->drawImage(rect, mScreenImage, rect);
//
//    return touched;
}
