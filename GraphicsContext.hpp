#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include "Device.hpp"

class ScanConverter;

class GraphicsContext
{
    friend class ScanConverter;
    
public:
    GraphicsContext();
    ~GraphicsContext();
    
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void drawLine(int x0, int y0, int x1, int y1);
    void drawCircle(int centerX, int centerY, int radius);

private:
    Device *device;
    ScanConverter *scanConverter;
};

#endif // GRAPHICSCONTEXT_H
