#ifndef SIZE_HPP
#define SIZE_HPP

class Size
{
public:
    Size() noexcept : _mWidth(0), _mHeight(0) { }
    Size(int width, int height) noexcept : _mWidth(width), _mHeight(height) { }
    
    inline int width() const noexcept { return _mWidth; }
    inline int height() const noexcept { return _mHeight; }
    
    inline bool isEmpty() const noexcept { return _mWidth < 1 || _mHeight < 1; }
    
private:
    int _mWidth;
    int _mHeight;
};

#endif // SIZE_HPP
