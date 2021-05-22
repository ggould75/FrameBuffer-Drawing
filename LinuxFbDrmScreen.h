#ifndef LINUXFBDRMSCREEN_H
#define LINUXFBDRMSCREEN_H

#include "FbScreen.hpp"

class LinuxFbDrmDevice;

class LinuxFbDrmScreen : public FbScreen
{
public:
    LinuxFbDrmScreen();
    ~LinuxFbDrmScreen();

    bool initialize() override;
    void redraw() override;
    void redraw(Image *image) override; // TODO: temporary, see base class
    
private:
    LinuxFbDrmDevice *_mDevice;
};

#endif // LINUXFBDRMSCREEN_H
