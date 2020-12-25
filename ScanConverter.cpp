#include "ScanConverter.hpp"
#include "GraphicsContext.hpp"

void ScanConverter::bresenham(GraphicsContext& context, int x0, int y0, int x1, int y1)
{
    int dx = abs(x0 - x1);
    int dy = abs(y0 - y1);
    
    // Start/end points are equal
    if (dx == 0 && dy == 0) {
        context.drawPixel(x0, y0, 0x00, 0xAA, 0xFF);
        return;
    } else if (dx == 0) {
        // Vertical line
        for (int y = min(y0, y1); y <= max(y0, y1); y++) {
            context.drawPixel(x0, y, 0x00, 0xAA, 0xFF);
        }
        return;
    } else if (dy == 0) {
        // Horizontal line
        for (int x = min(x0, x1); x <= max(x0, x1); x++) {
            context.drawPixel(x, y0, 0x00, 0xAA, 0xFF);
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

    context.drawPixel(x, y, 0x00, 0xAA, 0xFF);
 
    while (x < xEnd) {
        x++;
        if (d < 0) {
            d += eastIncrement;
        } else {
            y++;
            d += northEastIncrement;
        }

        context.drawPixel(x, y, 0x00, 0xAA, 0xFF);
    }
}

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

void ScanConverter::circleMidPoint(GraphicsContext& context, int centerX, int centerY, int radius)
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
