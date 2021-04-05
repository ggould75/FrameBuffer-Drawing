#ifndef RECT_HPP
#define RECT_HPP

#include "Size.hpp"

class Rect
{
public:
    constexpr Rect() noexcept;
    constexpr Rect(const Size &size) noexcept;
    Rect(int left, int top, int width, int height) noexcept 
        : _mX1(left), 
          _mY1(top), 
          _mX2(left + width - 1), 
          _mY2(top + height - 1) { }
//     Rect(Point origin, Size size) noexcept {
//         
//     }
    
    int left() const noexcept;
    int top() const noexcept;
    int width() const noexcept;
    int height() const noexcept;
    Size size() const noexcept;
    
private:
    int _mX1;
    int _mY1;
    int _mX2;
    int _mY2;
};

constexpr inline Rect::Rect() noexcept : _mX1(0), _mY1(0), _mX2(-1), _mY2(-1) { }

constexpr inline Rect::Rect(const Size &size) noexcept : _mX1(0), _mY1(0), _mX2(size.width() - 1), _mY2(size.height() - 1) { }

inline int Rect::left() const noexcept 
{ 
    return _mX1; 
}

inline int Rect::top() const noexcept 
{
    return _mY1; 
}

inline int Rect::width() const noexcept 
{
    return _mX2 - _mX1 + 1;
}

inline int Rect::height() const noexcept 
{
    return _mY2 - _mY1 + 1;
}

inline Size Rect::size() const noexcept 
{
    return Size(width(), height());
}

#endif // RECT_HPP
