#ifndef RECT_HPP
#define RECT_HPP

#include "Size.hpp"

class Rect
{
public:
    Rect() noexcept : _mX1(0), _mY1(0), _mX2(-1), _mY2(-1) { }
    Rect(const Size &size) noexcept : _mX1(0), _mY1(0), _mX2(size.width() - 1), _mY2(size.height() - 1) { }
    Rect(int left, int top, int width, int height) noexcept 
        : _mX1(left), 
          _mY1(top), 
          _mX2(left + width - 1), 
          _mY2(top + height - 1) { }
//     Rect(Point origin, Size size) noexcept {
//         
//     }
    
    inline int left() const noexcept { return _mX1; }
    inline int top() const noexcept { return _mY1; }
    
    inline int width() const noexcept {
        return _mX2 - _mX1 + 1;
    }
    
    inline int height() const noexcept {
        return _mY2 - _mY1 + 1;
    }
    
    inline Size size() const noexcept {
        return Size(width(), height());
    }
    
private:
    int _mX1;
    int _mY1;
    int _mX2;
    int _mY2;
};

#endif // RECT_HPP
