#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#include <stdint.h>

class GraphicsContextPrivate;

class GraphicsContext
{
    friend class GraphicsContextPrivate;
    
public:
    GraphicsContext();
    ~GraphicsContext();
    
    void setup();
    
    void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void drawLine(int x0, int y0, int x1, int y1);
    void drawCircle(int centerX, int centerY, int radius);
    
    void fillRect(int x0, int y0, int width, int height);
    
    void clear();

private:
    //Device *m_device;
    GraphicsContextPrivate *d_ptr;
};

#endif // GRAPHICSCONTEXT_H
