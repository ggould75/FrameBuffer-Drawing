#ifndef LINUXFBSCREEN_H
#define LINUXFBSCREEN_H

#include "FbScreen.hpp"
#include "Image.hpp"

class Painter;

class LinuxFbScreen : public FbScreen
{
public:
    LinuxFbScreen(short int fbNumber);
    ~LinuxFbScreen();
    
    bool initialize() override;
    void redraw() override;

private:
    struct {
        unsigned char *data;
        unsigned int bytesPerLine;
        int offset;
        int length;
    } _mMmap;
    
    int _mFbFd;
    short int _mfbNumber;
    
    Image _mFbScreenImage;
    Painter *_mBlitterPainter;
};

#endif // LINUXFBSCREEN_H
