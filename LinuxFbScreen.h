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
    void redraw(Image *image) override; // TODO: temporary, see base class
    void clearScreen();
    
private:
    struct {
        unsigned char *data;
        unsigned int bytesPerLine;
        int offset;
        int length;
    } _mMmap;
    
    int _mTtyFd;
    int _mOldTtyMode;
    bool _shouldSaveAndRestoreTtyMode;
    
    int _mFbFd;
    short int _mFbNumber;
    
    Image _mFbScreenImage;
    Painter *_mBlitterPainter;
};

#endif // LINUXFBSCREEN_H
