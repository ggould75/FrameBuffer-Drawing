#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include "Device.hpp"

class GraphicsContext
{
public:
    GraphicsContext();
    ~GraphicsContext();
    
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void drawLine(int x1, int y1, int x2, int y2);

private:
    Device *device;
};

#endif // GRAPHICSCONTEXT_H
