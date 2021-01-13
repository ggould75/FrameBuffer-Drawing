#include "GraphicsContext.hpp"
#include "GraphicsContext_p.hpp"

//GraphicsContext::GraphicsContext() : m_device(new LinuxFrameBuffer(0)), d_ptr(new GraphicsContextPrivate())
GraphicsContext::GraphicsContext() : d_ptr(new GraphicsContextPrivate())
{
    
}

GraphicsContext::~GraphicsContext()
{    
    delete d_ptr;
    //delete m_device;
}

void GraphicsContext::setup()
{
//     if (!m_device->initialize()) {
//         std::cerr << "Device not ready!" << std::endl;
//     }
}

void GraphicsContext::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    //m_device->drawPixel(x, y, r, g, b);
}

void GraphicsContext::drawLine(int x0, int y0, int x1, int y1)
{
    d_ptr->bresenham(*this, x0, y0, x1, y1);
}

void GraphicsContext::drawCircle(int centerX, int centerY, int radius)
{
    d_ptr->circleMidPoint(*this, centerX, centerY, radius);
}

void GraphicsContext::fillRect(int x0, int y0, int width, int height)
{
    for (int x = x0; x <= x0 + width; x++) {
        for (int y = y0; y <= y0 + height; y++) {
            //m_device->drawPixel(x, y, 0x00, 0xFF, 0x00);
        }
    }
}

void GraphicsContext::clear()
{
    //m_device->clearScreen();
}
