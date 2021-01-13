#ifndef SCREEN_H
#define SCREEN_H

class Screen
{
public:
    
    virtual ~Screen() = default;

    virtual bool initialize() = 0;
    
    virtual int depth() const = 0;
    
    // Possible virtual methods:
    // virtual Rect geometry() const = 0;
    // 
    // virtual Image::PixelFormat pixelFormat() const = 0;
    // 
    // virtual WindowList windows() const;
    //
    // virtual Cursor *cursor() const;
    //
    // virtual vector<Mode> modes() const;
    // virtual int currentMode() const;
    //
};

#endif // SCREEN_H
