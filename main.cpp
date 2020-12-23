//
//  main.cpp
//  LowLevelDrawing
//
//  Created by Marco Mussini on 22/12/2020.
//

#include "GraphicsContext.hpp"

int main(int argc, const char * argv[]) {

    GraphicsContext gc = GraphicsContext();
    gc.drawPixel(100, 200, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 201, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 202, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 203, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 204, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 205, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 206, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 207, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 208, 0xFF, 0x00, 0x00);
    gc.drawPixel(100, 209, 0xFF, 0x00, 0x00);
    gc.drawLine(0, 0, 100, 200);

    return 0;
}
