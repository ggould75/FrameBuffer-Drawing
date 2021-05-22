#ifndef FBSCREEN_H
#define FBSCREEN_H

#include "Screen.hpp"
#include "Image.hpp"

class Painter;

class FbScreen : public Screen
{
public:
    FbScreen();
    ~FbScreen();
    
    bool initialize() override;
    
    int depth() const override { return mDepth; }
    Rect geometry() const override { return mGeometry; }
    void setGeometry(const Rect &rect);
    
    virtual void redraw();
    
    // TODO: temporary until I have defined the concept of window
    // Once I have that, I'm supposed to do my drawing on the window's "backing store" and then take backingStore->image() and transfer that image to mScreenImage. Therefore eventually I should be able to replace this method with a simple redraw()...
    virtual void redraw(Image *image);
    
protected:
    Rect mGeometry;
    Image *mScreenImage;
    int mDepth; 
    Image::Format mFormat;
    
    void initializeCompositor();
    
private:
    Painter *_mPainter;
};

#endif // FBSCREEN_H
