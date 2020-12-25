#ifndef SCANCONVERTER_H
#define SCANCONVERTER_H

class Device;
class GraphicsContext;

class ScanConverter
{
public:
    void bresenham(GraphicsContext& context, int x0, int y0, int x1, int y1);
    void circleMidPoint(GraphicsContext& context, int centerX, int centerY, int radius);
};

#endif // SCANCONVERTER_H
