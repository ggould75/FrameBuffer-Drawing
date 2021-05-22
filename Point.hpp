#ifndef POINT_H
#define POINT_H

class Point
{
public:
    constexpr Point() noexcept;
    constexpr Point(int x, int y) noexcept;
    
    constexpr int x() noexcept;
    constexpr int y() noexcept;

private:
    int _mX;
    int _mY;
};

constexpr inline Point::Point() noexcept 
    : _mX(0), _mY(0) {}

constexpr inline Point::Point(int x, int y) noexcept 
    : _mX(x), _mY(y) {}

constexpr inline int Point::x() noexcept
{
    return _mX;
}

constexpr inline int Point::y() noexcept
{
    return _mY;
}

#endif // POINT_H
