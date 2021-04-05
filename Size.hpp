#ifndef SIZE_HPP
#define SIZE_HPP

class Size
{
public:
    constexpr Size() noexcept;
    constexpr Size(int width, int height) noexcept;
    
    constexpr int width() const noexcept;
    constexpr int height() const noexcept;
    
    constexpr bool isEmpty() const noexcept;
    
private:
    int _mWidth;
    int _mHeight;
};

constexpr Size::Size() noexcept : _mWidth(0), _mHeight(0) { }

constexpr Size::Size(int width, int height) noexcept : _mWidth(width), _mHeight(height) { }

constexpr inline int Size::width() const noexcept
{ 
    return _mWidth; 
}

constexpr inline int Size::height() const noexcept 
{ 
    return _mHeight; 
}
    
constexpr inline bool Size::isEmpty() const noexcept 
{ 
    return _mWidth < 1 || _mHeight < 1; 
}
    
#endif // SIZE_HPP
