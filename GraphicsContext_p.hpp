#ifndef GRAPHICSCONTEXTPRIVATE_H
#define GRAPHICSCONTEXTPRIVATE_H

class GraphicsContext;

class GraphicsContextPrivate
{
public:
    void bresenham(GraphicsContext& context, int x0, int y0, int x1, int y1);
    void fillRect(GraphicsContext& context, int x0, int y0, int x1, int y1);
    void circleMidPoint(GraphicsContext& context, int centerX, int centerY, int radius);
};

#endif // GRAPHICSCONTEXTPRIVATE_H
