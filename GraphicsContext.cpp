#include "GraphicsContext.hpp"
#include "FrameBuffer.hpp"
#include "ScanConverter.hpp"

GraphicsContext::GraphicsContext()
{
    device = new FrameBuffer(0);
    device->openDevice();
    scanConverter = new ScanConverter();
}

GraphicsContext::~GraphicsContext()
{
    delete scanConverter;
    delete device;
}

void GraphicsContext::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    device->drawPixel(x, y, r, g, b);
}

void GraphicsContext::drawLine(int x0, int y0, int x1, int y1)
{
    scanConverter->bresenham(*this, x0, y0, x1, y1);
}

void GraphicsContext::drawCircle(int centerX, int centerY, int radius)
{
    scanConverter->circleMidPoint(*this, centerX, centerY, radius);
}



