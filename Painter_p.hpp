#ifndef PAINTERPRIVATE_HPP
#define PAINTERPRIVATE_HPP

class Drawable;

class PainterPrivate
{
public:
    explicit PainterPrivate(Drawable *drawable) : mDrawable(drawable) { }
    
    Drawable *mDrawable;
};

#endif // PAINTERPRIVATE_HPP
