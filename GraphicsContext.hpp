#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include "Device.hpp"

class GraphicsContext
{
public:
    GraphicsContext();
    ~GraphicsContext();
    
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void drawLine(int xa, int ya, int xb, int yb);

private:
    Device *device;
};

#endif // GRAPHICSCONTEXT_H
