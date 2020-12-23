#include "GraphicsContext.hpp"

int main(int argc, const char * argv[]) {

    GraphicsContext gc = GraphicsContext();
    
    gc.drawPixel(100, 200, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 201, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 202, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 203, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 204, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 205, 0xFF, 0x00, 0x00);
    
    gc.drawLine(900, 400, 900, 500);
    
    gc.drawLine(800, 300, 1800, 300);
    gc.drawLine(1800, 300, 1800, 1000);
    gc.drawLine(1800, 1000, 800, 1000);
    gc.drawLine(800, 1000, 800, 300);
    
    return 0;
}
