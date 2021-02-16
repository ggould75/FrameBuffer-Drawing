#ifndef PAINTER_HPP
#define PAINTER_HPP

class Drawable;
class PainterPrivate;

class Painter
{
public:
    explicit Painter(Drawable *drawable);
    ~Painter();

    bool begin();
    bool end();
    
    void setPen(int red, int green, int blue);
    
    void drawPoint(int x, int y);
    void drawLine(int x0, int y0, int x1, int y1);
    
private:
    PainterPrivate *_dPtr;
};

#endif // PAINTER_HPP
