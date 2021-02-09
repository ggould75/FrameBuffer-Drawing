#ifndef DRAWENGINE_HPP
#define DRAWENGINE_HPP

class DrawEngine
{
public:
    virtual ~DrawEngine() = default;

    virtual void drawPoint(int x, int y) = 0;
    virtual void drawLine(int x0, int y0, int x1, int y1) = 0;
    
    // TODO
    // virtual void setPen(Pen pen) = 0;
    // virtual void setBrush(Brush brush) = 0;
    // ... etc...
};

class Image;

class RasterBuffer
{
public:
    RasterBuffer();
    
    void prepareFromImage(Image *image);
    
private:
    int _mWidth;
    int _mHeight;
    unsigned char *_mBuffer;
};

class Drawable;

class RasterDrawEngine : public DrawEngine
{
public:
    RasterDrawEngine(Drawable *drawable);
    ~RasterDrawEngine();

    void drawPoint(int x, int y) override;
    void drawLine(int x0, int y0, int x1, int y1) override;
    
private:
    void init();
    
    Drawable *_mDrawable;
    RasterBuffer *_mRasterBuffer;
};

#endif // DRAWENGINE_HPP
