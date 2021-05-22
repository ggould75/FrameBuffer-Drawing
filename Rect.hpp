#ifndef RECT_HPP
#define RECT_HPP

#include "Size.hpp"
#include "Point.hpp"

class Rect
{
public:
    constexpr Rect() noexcept;
    constexpr Rect(const Size &size) noexcept;
    constexpr Rect(int left, int top, int width, int height) noexcept;
    constexpr Rect(Point &topLeftOrigin, Size &size) noexcept;

    constexpr int left() const noexcept;
    constexpr int top() const noexcept;
    constexpr int width() const noexcept;
    constexpr int height() const noexcept;
    constexpr Size size() const noexcept;
    
private:
    int _mX1;
    int _mY1;
    int _mX2;
    int _mY2;
};

constexpr inline Rect::Rect() noexcept 
    : _mX1(0), _mY1(0), _mX2(-1), _mY2(-1) {}

constexpr inline Rect::Rect(const Size &size) noexcept 
    : _mX1(0), _mY1(0), _mX2(size.width() - 1), _mY2(size.height() - 1) {}

constexpr inline Rect::Rect(int left, int top, int width, int height) noexcept
    : _mX1(left), _mY1(top), _mX2(left + width - 1), _mY2(top + height - 1) {}

constexpr inline Rect::Rect(Point &topLeftOrigin, Size &size) noexcept
    : _mX1(topLeftOrigin.x()), _mY1(topLeftOrigin.y()), _mX2(topLeftOrigin.x() + size.width()), _mY2(topLeftOrigin.y() + size.height()) {}

constexpr inline int Rect::left() const noexcept 
{ 
    return _mX1;
}

constexpr inline int Rect::top() const noexcept 
{
    return _mY1;
}

constexpr inline int Rect::width() const noexcept 
{
    return _mX2 - _mX1 + 1;
}

constexpr inline int Rect::height() const noexcept 
{
    return _mY2 - _mY1 + 1;
}

constexpr inline Size Rect::size() const noexcept 
{
    return Size(width(), height());
}

#endif // RECT_HPP
