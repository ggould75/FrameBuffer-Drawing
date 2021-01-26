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
    _mDevice = new LinuxFbDrmDevice(1);
    
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