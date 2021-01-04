#include "GraphicsContext.hpp"
#include "FrameBuffer.hpp"
#include "GraphicsContext_p.hpp"

GraphicsContext::GraphicsContext() : device(new FrameBuffer(0)), d_ptr(new GraphicsContextPrivate())
{
    device->openDevice();
}

GraphicsContext::~GraphicsContext()
{
    delete d_ptr;
    delete device;
}

void GraphicsContext::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    device->drawPixel(x, y, r, g, b);
}

void GraphicsContext::drawLine(int x0, int y0, int x1, int y1)
{
    d_ptr->bresenham(*this, x0, y0, x1, y1);
}

void GraphicsContext::drawCircle(int centerX, int centerY, int radius)
{
    d_ptr->circleMidPoint(*this, centerX, centerY, radius);
}
