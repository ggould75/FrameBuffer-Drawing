#include "GraphicsContext.hpp"
#include "GraphicsContext_p.hpp"
#include "FrameBuffer.hpp"

GraphicsContext::GraphicsContext() : m_device(new FrameBuffer(0)), d_ptr(new GraphicsContextPrivate())
{
    
}

GraphicsContext::~GraphicsContext()
{    
    delete d_ptr;
    delete m_device;
}

void GraphicsContext::setup()
{
    if (!m_device->initialize()) {
        cout << "Device not ready!" << endl;
    }
}

void GraphicsContext::drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    m_device->drawPixel(x, y, r, g, b);
}

void GraphicsContext::drawLine(int x0, int y0, int x1, int y1)
{
    d_ptr->bresenham(*this, x0, y0, x1, y1);
}

void GraphicsContext::drawCircle(int centerX, int centerY, int radius)
{
    d_ptr->circleMidPoint(*this, centerX, centerY, radius);
}

void GraphicsContext::clear()
{
    m_device->clearScreen();
}
