#ifndef FBSCREEN_H
#define FBSCREEN_H

#include "Screen.hpp"

class Image;
class Painter;

class FbScreen : public Screen
{
public:
    FbScreen();
    ~FbScreen();
    
    bool initialize() override;
    
    int depth() const override { return mDepth; }
    
    virtual void redraw();
    
protected:
    Image *mScreenImage;
    int mDepth;
    
private:
    Painter *_mPainter;
};

#endif // FBSCREEN_H
