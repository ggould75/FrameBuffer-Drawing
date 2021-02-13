#include "Painter.hpp"
#include "Painter_p.hpp"
#include "DrawEngine.hpp"
#include "Drawable.hpp"
#include "Image.hpp"

#include <iostream>

using namespace std;

Painter::Painter(Drawable *drawable) : _dPtr(new PainterPrivate(drawable))
{

}

Painter::~Painter()
{
    delete _dPtr;
}

bool Painter::begin()
{
    _dPtr->mDrawEngine = _dPtr->mDrawable->drawEngine();
    if (_dPtr->mDrawEngine == nullptr) {
        cerr << "Drawable has no engine defined!" << endl;
        return false;
    }
    
    // TODO: I may need to add a type to drawable, switch over it and use static cast (for dealing with multiple types)
    Image *imageDrawable = dynamic_cast<Image *>(_dPtr->mDrawable);
    if (imageDrawable && imageDrawable->isNull()) {
        cerr << "Image is not properly initialized" << endl;
        return false;
    }
    
    return true;
}
    
bool Painter::end()
{
    return true;
}

void Painter::drawPoint(int x, int y)
{
    if (_dPtr->mDrawEngine == nullptr) {
        return;
    }

    _dPtr->mDrawEngine->drawPoint(x, y);
}
    
void Painter::drawLine(int x0, int y0, int x1, int y1)
{
    if (_dPtr->mDrawEngine == nullptr) {
        return;
    }
    
    _dPtr->mDrawEngine->drawLine(x0, y0, x1, y1);
}
