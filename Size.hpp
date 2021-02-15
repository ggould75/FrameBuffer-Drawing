#ifndef SIZE_HPP
#define SIZE_HPP

class Size
{
public:
    Size() noexcept : _mWidth(0), _mHeight(0) { }
    Size(int width, int height) noexcept : _mWidth(width), _mHeight(height) { }
    
    int width() const noexcept;
    int height() const noexcept;
    
    bool isEmpty() const noexcept;
    
private:
    int _mWidth;
    int _mHeight;
};

inline int Size::width() const noexcept
{ 
    return _mWidth; 
}

inline int Size::height() const noexcept 
{ 
    return _mHeight; 
}
    
inline bool Size::isEmpty() const noexcept 
{ 
    return _mWidth < 1 || _mHeight < 1; 
}
    
#endif // SIZE_HPP
