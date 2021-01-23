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
    Rect geometry() const override { return mGeometry; }
    void setGeometry(const Rect &rect);
    
    virtual void redraw();
    
protected:
    Rect mGeometry;
    Image *mScreenImage;
    int mDepth;
    
private:
    Painter *_mPainter;
};

#endif // FBSCREEN_H
