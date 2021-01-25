#include "LinuxFbScreen.h"
#include "Painter.hpp"
#include "Rect.hpp"

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <iostream>

#include <linux/fb.h>

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

LinuxFbScreen::LinuxFbScreen(short int fbNumber) 
    : _mFbNumber(fbNumber),
      _mFbFd(-1), 
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
    
    delete _mBlitterPainter;
}

bool LinuxFbScreen::initialize() 
{
    char devicePath[12];
    sprintf(devicePath, "/dev/fb%d", _mFbNumber);
    struct stat sb;
    int openFlags = O_RDONLY | O_CLOEXEC;
    
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
    Rect relativeGeometry = Rect(0, 0, absoluteGeometry.size().width(), absoluteGeometry.size().height());
    mGeometry = relativeGeometry;
    // TODO: mFormat = determineFormat(variableInfo, mDepth);
    
    _mMmap.bytesPerLine = fixedInfo.line_length;
    _mMmap.length = fixedInfo.smem_len;
    
    unsigned char *data = (unsigned char *)mmap(0, _mMmap.length, PROT_READ | PROT_WRITE, MAP_SHARED, _mFbFd, (off_t)0);
    if (data == MAP_FAILED) {
        cerr << "Failed to map fb (" << errno << "): " << strerror(errno) << endl;
        return false;
    }
    
    _mMmap.offset = absoluteGeometry.top() * _mMmap.bytesPerLine + absoluteGeometry.left() * mDepth / 8;
    _mMmap.data = data + _mMmap.offset;
    
    FbScreen::initializeCompositor();
    _mFbScreenImage = Image(); // TODO: init with (_mMmap.data, absoluteGeometry.width(), absoluteGeometry.height(), mMmap.bytesPerLine, 
    
    // TODO: switch to graphics mode
    // TODO: blank screen
    
    return true;
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
