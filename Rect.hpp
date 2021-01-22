#ifndef RECT_HPP
#define RECT_HPP

#include "Size.hpp"

class Rect
{
public:
    Rect() noexcept : _mX1(0), _mY1(0), _mX2(-1), _mY2(-1) { }
    Rect(const Size &size) noexcept : _mX1(0), _mY1(0), _mX2(size.width() - 1), _mY2(size.height() - 1) { }
    
private:
    int _mX1;
    int _mY1;
    int _mX2;
    int _mY2;
};

#endif // RECT_HPP
