#include "DrawEngine.hpp"
#include "Image.hpp"

#include <iostream>

RasterBuffer::RasterBuffer() : _mWidth(0), _mHeight(0), _mDepth(0), _mBytesPerLine(0), _mBuffer(nullptr)
{
}

void RasterBuffer::prepareFromImage(Image *image)
{
    _mWidth = image->width();
    _mHeight = image->height();
    _mDepth = image->depth();
    _mBytesPerLine = image->bytesPerLine();
    _mBuffer = image->bytes();
}

RasterDrawEngine::RasterDrawEngine(Drawable *drawable) 
    : _mDrawable(drawable), _mRasterBuffer(nullptr), _mPixelValue(0)
{
    init();
}

void RasterDrawEngine::init()
{
    _mRasterBuffer = new RasterBuffer();
    
    Image *imageDrawable = dynamic_cast<Image *>(_mDrawable);
    if (imageDrawable) {
        _mRasterBuffer->prepareFromImage(imageDrawable);
    } else {
        std::cerr << "Unimplemented drawable subclass" << std::endl;
    }
}

RasterDrawEngine::~RasterDrawEngine()
{
    _mDrawable = nullptr;
    delete _mRasterBuffer;
}

void RasterDrawEngine::setPen(uint8_t red, uint8_t green, uint8_t blue)
{
    _mPixelValue = (red << 16) | (green << 8) | blue; // FIXME: works only for 32bpp!
}

void RasterDrawEngine::drawPoint(int x, int y)
{
    drawPoint(x, _mRasterBuffer->scanLine(y));
}

void RasterDrawEngine::drawLine(int x0, int y0, int x1, int y1)
{
    int dx = abs(x0 - x1);
    int dy = abs(y0 - y1);
    
    // Start/end points are equal
    if (dx == 0 && dy == 0) {
        drawPoint(x0, _mRasterBuffer->scanLine(y0));
        return;
    } else if (dx == 0) {
        // Vertical line_mBytesPerLine
        for (int y = std::min(y0, y1); y <= std::max(y0, y1); y++) {
            drawPoint(x0, _mRasterBuffer->scanLine(y));
        }
        return;
    } else if (dy == 0) {
        // Horizontal line
        for (int x = std::min(x0, x1); x <= std::max(x0, x1); x++) {
            drawPoint(x, _mRasterBuffer->scanLine(y0));
        }
        return;
    }
    
    int d = 2 * dy - dx;
    int eastIncrement = 2 * dy;
    int northEastIncrement = 2 * (dy - dx);
    int x, y, xEnd;

    if (x0 > x1) {
        x = x1;
        y = y1;
        xEnd = x0;
    } else {
        x = x0;
        y = y0;
        xEnd = x1;
    }

    unsigned char *scanLineY = _mRasterBuffer->scanLine(y);
    
    drawPoint(x, scanLineY);
 
    while (x < xEnd) {
        x++;
        if (d < 0) {
            d += eastIncrement;
        } else {
            y++;
            d += northEastIncrement;
        }

        drawPoint(x, scanLineY);
    }
}

void RasterDrawEngine::drawPoint(int x, unsigned char *scanline)
{
    int depth = _mRasterBuffer->depth();
    switch (depth) {
        case 32:
            scanline[x] = _mPixelValue;
        case 16:
            scanline[x] = _mPixelValue & 0xffff;
    }
}

/** 
 * Just moving this old code here for now, until I understand how to deal with rasterization properly...
 * 

void circlePlotPoints(GraphicsContext& context, int centerX, int centerY, int x, int y)
{
    context.drawPixel(centerX + x, centerY + y, 0xFF, 0x00, 0x00);
    context.drawPixel(centerX - x, centerY + y, 0xFF, 0x00, 0x00);
    context.drawPixel(centerX + x, centerY - y, 0xFF, 0x00, 0x00);
    context.drawPixel(centerX - x, centerY - y, 0xFF, 0x00, 0x00);
    context.drawPixel(centerX + y, centerY + x, 0xFF, 0x00, 0x00);
    context.drawPixel(centerX - y, centerY + x, 0xFF, 0x00, 0x00);
    context.drawPixel(centerX + y, centerY - x, 0xFF, 0x00, 0x00);
    context.drawPixel(centerX - y, centerY - x, 0xFF, 0x00, 0x00);
}

void GraphicsContextPrivate::circleMidPoint(GraphicsContext& context, int centerX, int centerY, int radius)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    int deltaE = 3;
    int deltaSE = -2 * radius + 5;
    
    circlePlotPoints(context, centerX, centerY, x, y);
    
    while (y > x) {
        if (d < 0) {
            d += deltaE;
            deltaE += 2;
            deltaSE += 2;
        } else {
            d += deltaSE;
            deltaE += 2;
            deltaSE += 4;
            y--;
        }
        
        x++;
        
        circlePlotPoints(context, centerX, centerY, x, y);
    }
}
*/
