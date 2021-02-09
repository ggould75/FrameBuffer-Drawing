#include "Painter.hpp"
#include "Painter_p.hpp"

Painter::Painter(Drawable *drawable) : _dPtr(new PainterPrivate(drawable))
{

}

Painter::~Painter()
{
    delete _dPtr;
}

bool begin()
{
    // TODO: create/assign engine
    
    return true;
}
    
bool end()
{
    return true;
}

void Painter::drawPoint(int x, int y)
{
    // TODO: redirect call to the engine
}
    
void Painter::drawLine(int x0, int y0, int x1, int y1)
{
    // TODO: redirect call to the engine
}
