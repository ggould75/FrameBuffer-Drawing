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

void GraphicsContext::drawLine(int xa, int ya, int xb, int yb)
{
    int dx = abs(xa - xb);
    int dy = abs(ya - yb);
    int p = 2 * dy - dx;
    int twoDy = 2 * dy;
    int twoDyDx = 2 * (dy - dx);
    int x, y, xEnd;

    if (xa > xb) {
        x = xb;
        y = yb;
        xEnd = xa;
    } else {
        x = xa;
        y = ya;
        xEnd = xb;
    }

    drawPixel(x, y, 0x00, 0xAA, 0xFF);
 
    while (x < xEnd) {
        x++;
        if (p < 0) {
            p += twoDy;
        } else {
            y++;
            p += twoDyDx;
        }

        drawPixel(x, y, 0x00, 0xAA, 0xFF);
    }
}

