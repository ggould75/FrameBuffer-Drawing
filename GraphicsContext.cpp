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

void GraphicsContext::drawLine(int x0, int y0, int x1, int y1)
{
    int dx = abs(x0 - x1);
    int dy = abs(y0 - y1);
    cout << "dx: " << dx << ", dy: " << dy << endl;
    
    // Start/end points are equal
    if (dx == 0 && dy == 0) {
        drawPixel(x0, y0, 0x00, 0xAA, 0xFF);
        return;
    } else if (dx == 0) {
        // Vertical line
        for (int y = min(y0, y1); y <= max(y0, y1); y++) {
            drawPixel(x0, y, 0x00, 0xAA, 0xFF);
        }
        return;
    } else if (dy == 0) {
        // Horizontal line
        for (int x = min(x0, x1); x <= max(x0, x1); x++) {
            drawPixel(x, y0, 0x00, 0xAA, 0xFF);
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

    drawPixel(x, y, 0x00, 0xAA, 0xFF);
 
    while (x < xEnd) {
        x++;
        if (d < 0) {
            d += eastIncrement;
        } else {
            y++;
            d += northEastIncrement;
        }

        drawPixel(x, y, 0x00, 0xAA, 0xFF);
    }
}

