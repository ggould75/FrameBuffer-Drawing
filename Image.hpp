#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "Drawable.hpp"

struct ImageDataPrivate;
class Size;

class Image : public Drawable
{
public:
    enum Format {
        Format_RGB16,
        Format_Invalid,
    };
    
    Image() noexcept;
    Image(const Size &size, Format format);
    Image(unsigned char *data, int width, int height, int bytesPerLine, Format format);
    ~Image();
    
    int width() const;
    int height() const;
    int depth() const;
    Format format() const;
    unsigned char *bytes();
    
    DrawEngine *drawEngine() const override;
    
    bool isNull() const;
    
private:
    ImageDataPrivate *_dPtr;
};

#endif // IMAGE_HPP
