#ifndef IMAGEDATAPRIVATE_H
#define IMAGEDATAPRIVATE_H

#include "Image.hpp"

#include <cstddef>

struct ImageDataPrivate
{
    ImageDataPrivate();
    ~ImageDataPrivate();
    
    static ImageDataPrivate *create(const Size &size, Image::Format format);
    static ImageDataPrivate *create(unsigned char *data, int width, int height, int bytesPerLine, Image::Format format);
    
    struct ImageSizeParameters {
        std::size_t bytesPerLine;
        std::size_t totalBytes;
        bool isValid() const { return bytesPerLine > 0 && totalBytes > 0; }
    };
    
    static ImageSizeParameters calculateImageParameters(int width, int height, int depth);
    static int depthFromImageFormat(Image::Format format);
    
    int width;
    int height;
    int depth;
    
    std::size_t bytesPerLine;
    std::size_t totalBytes;
    unsigned char *data;
    bool isDataOwner;
    
    Image::Format format;
};

#endif // IMAGEDATAPRIVATE_H
