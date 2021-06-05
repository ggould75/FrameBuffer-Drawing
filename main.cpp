#include "GraphicsContext.hpp"
#include "common.h"

#include <unistd.h>

int main(int argc, const char * argv[]) 
{
    GraphicsContext gc = GraphicsContext();
    gc.setup();
 
 	gc.clear();

    gc.drawPixel(100, 200, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 201, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 202, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 203, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 204, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 205, 0xFF, 0x00, 0x00);

    // vertical lines
    gc.drawLine(900, 400, 900, 600);
    gc.drawLine(800, 600, 800, 400);
    // horizontal lines
    gc.drawLine(1000, 400, 1400, 400);
    // dots
    gc.drawLine(500, 500, 500, 500);
    gc.drawLine(900, 200, 900, 200);
    
    // rectangle
//    gc.drawLine(800, 300, 1800, 300);
//    gc.drawLine(1800, 300, 1800, 1000);
//    gc.drawLine(1800, 1000, 800, 1000);
//    gc.drawLine(800, 1000, 800, 300);
    
    gc.drawCircle(500, 200, 100);
    
    sleep(2);
    
//    for (int i = 0; i < 500; i++) {
//        gc.fillRect(2000, 300, 300, 150);
//        gc.clear();
//    }
    
    return 0;
}
