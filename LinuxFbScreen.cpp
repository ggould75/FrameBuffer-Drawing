#include "LinuxFbScreen.h"
#include "Painter.hpp"

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

LinuxFbScreen::LinuxFbScreen(short int fbNumber) 
    : _mfbNumber(fbNumber),
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
