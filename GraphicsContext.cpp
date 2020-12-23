#include "GraphicsContext.hpp"
#include "FrameBuffer.hpp"

GraphicsContext::GraphicsContext()
{
    device = new FrameBuffer(0);
    device->openDevice();
}

GraphicsContext::~GraphicsContext()
{
    delete device;
}

void GraphicsContext::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    device->drawPixel(x, y, r, g, b);
}

void GraphicsContext::drawLine(int x1, int y1, int x2, int y2)
{
    // TODO: use device->drawPixel(x, y) to draw all pixels...
}

