#include <cassert>  
#include <iostream>

#include "LinuxFbDrmScreen.h"
#include "LinuxFbDrmDevice.h"

using namespace std;

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

    if (_mDevice->outputCount() == 0) {
        cerr << "No available output screens" << endl;
        return false;
    }

    LinuxFbDrmDevice::Output *output(_mDevice->output(0));
    mGeometry = Rect(output->resolution);
    mDepth = depthForDrmFormat(output->drmFormat);
    mFormat = imageFormatForDrmFormat(output->drmFormat);

    FbScreen::initializeCompositor();

    return true;
}

void LinuxFbDrmScreen::redraw()
{

}

void LinuxFbDrmScreen::redraw(Image *image)
{
    FbScreen::redraw(image);
    
//    QLinuxFbDevice::Output *output(m_device->output(0));
//
//    for (int i = 0; i < BUFFER_COUNT; ++i)
//        output->dirty[i] += dirty;
//
//    if (output->fb[output->backFb].wrapper.isNull())
//        return dirty;
//
//    QPainter pntr(&output->fb[output->backFb].wrapper);
//    // Image has alpha but no need for blending at this stage.
//    // Do not waste time with the default SourceOver.
//    pntr.setCompositionMode(QPainter::CompositionMode_Source);
//    for (const QRect &rect : qAsConst(output->dirty[output->backFb]))
//        pntr.drawImage(rect, mScreenImage, rect);
//    pntr.end();
//
//    output->dirty[output->backFb] = QRegion();
//
//    m_device->swapBuffers(output);
}
