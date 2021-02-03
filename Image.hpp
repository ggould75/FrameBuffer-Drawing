#ifndef IMAGE_HPP
#define IMAGE_HPP

class Size;
struct ImageDataPrivate;

class Image
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
    
private:
    ImageDataPrivate *_dPtr;
};

#endif // IMAGE_HPP
