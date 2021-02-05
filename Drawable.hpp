#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

class DrawEngine;

class Drawable
{
public:
    virtual ~Drawable() = default;
    
    virtual DrawEngine *drawEngine() const = 0;
};

#endif // DRAWABLE_HPP
