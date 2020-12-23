//
//  main.cpp
//  LowLevelDrawing
//
//  Created by Marco Mussini on 22/12/2020.
//

#include <iostream>

#include "Device.hpp"

int main(int argc, const char * argv[]) {

    GraphicContext gc = GraphicContext();
    gc.drawPixel(100, 200);
    gc.drawPixel(100, 201);
    gc.drawPixel(100, 202);
    gc.drawPixel(100, 203);
    gc.drawPixel(100, 204);
    gc.drawLine(0, 0, 100, 200);

    return 0;
}
