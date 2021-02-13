#ifndef PAINTERPRIVATE_HPP
#define PAINTERPRIVATE_HPP

class Drawable;
class DrawEngine;

class PainterPrivate
{
public:
    explicit PainterPrivate(Drawable *drawable) : mDrawable(drawable), mDrawEngine(nullptr) { }
    
    Drawable *mDrawable;
    DrawEngine *mDrawEngine;
};

#endif // PAINTERPRIVATE_HPP
