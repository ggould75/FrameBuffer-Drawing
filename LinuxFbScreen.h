#ifndef LINUXFBSCREEN_H
#define LINUXFBSCREEN_H

#include "FbScreen.hpp"

class LinuxFbScreen : public FbScreen
{
public:
    LinuxFbScreen();
    ~LinuxFbScreen();
    
    bool initialize() override;
    void redraw() override;
};

#endif // LINUXFBSCREEN_H
