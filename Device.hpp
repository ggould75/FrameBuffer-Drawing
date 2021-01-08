#ifndef Device_hpp
#define Device_hpp

#include <iostream>

using namespace std;

class Device
{
public:
    virtual ~Device() = default;
    virtual bool initialize() = 0;
    
    virtual void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void clearScreen() = 0;
    
// TODO: should not be visible in interface. Perhaps better to create a pure-virtual class for the private setup that all devices implement as d_ptr?
protected:
    virtual int open() = 0;
    virtual void close() = 0;
    virtual bool createFrameBuffer(int fd) = 0;
    virtual void swapBuffer() = 0;
};

#endif /* Device_hpp */
