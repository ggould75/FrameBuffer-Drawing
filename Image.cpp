#include "Image.hpp"
#include "ImageData_p.hpp"
#include "Size.hpp"

#include <stdlib.h>

Image::Image() noexcept
{

}

Image::Image(const Size &size, Format format)
{
    _dPtr = ImageDataPrivate::create(size, format);
}

Image::Image(unsigned char *data, int width, int height, int bytesPerLine, Format format)
{
    _dPtr = ImageDataPrivate::create(data, width, height, bytesPerLine, format);
}

Image::~Image()
{
    delete _dPtr;
}

ImageDataPrivate::ImageDataPrivate() :
    width(0), height(0), depth(0), bytesPerLine(0), totalBytes(0),
    data(nullptr), isDataOwner(true),
    format(Image::Format_RGB16)
{
}

ImageDataPrivate::~ImageDataPrivate()
{
    if (data && isDataOwner) {
        free(data);
    }
}

ImageDataPrivate * ImageDataPrivate::create(const Size &size, Image::Format format)
{
    if (size.isEmpty() || format == Image::Format_Invalid) {
        return nullptr;
    }
    
    int width = size.width();
    int height = size.height();
    int depth = depthFromImageFormat(format);
    auto imageParams = ImageDataPrivate::calculateImageParameters(width, height, depth);
    if (!imageParams.isValid()) {
        return nullptr;
    }
    
    ImageDataPrivate *imageData = new ImageDataPrivate();
    imageData->width = width;
    imageData->height = height;
    imageData->depth = depth;
    imageData->format = format;
    imageData->bytesPerLine = imageParams.bytesPerLine;
    imageData->totalBytes = imageParams.totalBytes;
    
    imageData->data = (unsigned char *)malloc(imageData->totalBytes);
    if (!imageData->data) {
        return nullptr;
    }
    
    return imageData;
}

ImageDataPrivate * ImageDataPrivate::create(unsigned char *data, int width, int height, int bytesPerLine, 
                                            Image::Format format)
{
    if (width <= 0 || height <= 0 || !data || format == Image::Format_Invalid) {
        return nullptr;
    }
    
    int depth = depthFromImageFormat(format);
    const int minBytesPerLine = (width * depth + 7) / 8;
    if (bytesPerLine < minBytesPerLine) {
        return nullptr;
    }
    std::size_t totalBytes = bytesPerLine * height; 
    
    ImageDataPrivate *imageData = new ImageDataPrivate();
    imageData->width = width;
    imageData->height = height;
    imageData->depth = depth;
    imageData->format = format;
    imageData->bytesPerLine = bytesPerLine;
    imageData->totalBytes = totalBytes;    
    imageData->data = data;
    imageData->isDataOwner = false;
    
    return imageData;
}
    
ImageDataPrivate::ImageSizeParameters ImageDataPrivate::calculateImageParameters(int width, int height, int depth)
{
    // Note: I should check for overflow!
    
    std::size_t bytesPerLine = width * depth;
    // Ensure is a multiple of 4
    bytesPerLine = (bytesPerLine >> 5) << 2;
    
    std::size_t totalBytes = bytesPerLine * height;
    
    return { bytesPerLine, totalBytes };
}

int ImageDataPrivate::depthFromImageFormat(Image::Format format)
{
    int depth = 0;
    
    switch (format) {
        case Image::Format_RGB16:
            depth = 16;
            break;
        default:
            break;
    }
    
    return depth;
}
