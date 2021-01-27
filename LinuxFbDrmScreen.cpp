#include "LinuxFbDrmScreen.h"
#include "LinuxFbDrmDevice.h"

LinuxFbDrmScreen::LinuxFbDrmScreen() : _mDevice(nullptr) { }

LinuxFbDrmScreen::~LinuxFbDrmScreen()
{
    if (_mDevice) {
        _mDevice->destroyFramebuffers();
        _mDevice->close();
        delete _mDevice;
    }
}

bool LinuxFbDrmScreen::initialize()
{
    // FIXME: the working dri card seems to be switching between /dev/dri/card0 and /dev/dri/card1
    // at every system restart
    _mDevice = new LinuxFbDrmDevice(0);
    
    if (!_mDevice->open()) {
        return false;
    }
    
    _mDevice->createScreens();
    _mDevice->createFramebuffers();
    _mDevice->setMode();

    if (_mDevice->outputCount() > 0) {
        LinuxFbDrmDevice::Output *output(_mDevice->output(0));
        mGeometry = Rect(output->resolution);
    }
    
    // TODO: set mDepth, mFormat, mScreenImage
    
    return true;
}

void LinuxFbDrmScreen::redraw()
{

}
