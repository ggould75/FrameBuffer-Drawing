#ifndef Device_hpp
#define Device_hpp

#include <iostream>

using namespace std;

class Device
{
public:
    virtual ~Device() = default;
    virtual void openDevice() = 0;
    virtual void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual uint32_t pixelColorFromRGBComponents(uint8_t r, uint8_t g, uint8_t b) = 0;
};

#endif /* Device_hpp */
