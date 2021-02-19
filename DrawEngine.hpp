#ifndef DRAWENGINE_HPP
#define DRAWENGINE_HPP

#include <stdint.h>

#include <cassert>

class DrawEngine
{
public:
    virtual ~DrawEngine() = default;

    // TODO: Eventually I will need to encapsulate color/pen info in a specific class
    virtual void setPen(uint8_t red, uint8_t green, uint8_t blue) = 0;
    // TODO
    // virtual void setBrush(Brush brush) = 0;
    // ... etc...
    
    virtual void drawPoint(int x, int y) = 0;
    virtual void drawLine(int x0, int y0, int x1, int y1) = 0;
};

class Image;

class RasterBuffer
{
public:
    RasterBuffer();
    
    void prepareFromImage(Image *image);
    
    unsigned char *scanLine(int y);
    int depth() const;
    
private:
    int _mWidth;
    int _mHeight;
    int _mDepth;
    int _mBytesPerLine;
    unsigned char *_mBuffer;
};

inline unsigned char *RasterBuffer::scanLine(int y)
{
    assert(y >= 0);
    assert(y < _mHeight);
    
    return _mBuffer + y * _mBytesPerLine;
}

inline int RasterBuffer::depth() const
{
    return _mDepth;
}

class Drawable;

class RasterDrawEngine : public DrawEngine
{
public:
    RasterDrawEngine(Drawable *drawable);
    ~RasterDrawEngine();

    void setPen(uint8_t red, uint8_t green, uint8_t blue) override;
    
    void drawPoint(int x, int y) override;
    void drawLine(int x0, int y0, int x1, int y1) override;
    
private:
    void init();
    void drawPoint(int x, unsigned char *scanline);
    
    Drawable *_mDrawable;
    RasterBuffer *_mRasterBuffer;
    
    unsigned long int _mPixelValue;
};

#endif // DRAWENGINE_HPP
