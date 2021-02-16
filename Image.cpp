#include "Image.hpp"
#include "ImageData_p.hpp"
#include "DrawEngine.hpp"
#include "Size.hpp"

#include <stdlib.h>

Image::Image() noexcept : _dPtr(nullptr)
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

int Image::width() const
{
    return _dPtr ? _dPtr->width : 0;
}

int Image::height() const
{
    return _dPtr ? _dPtr->height : 0;
}

int Image::depth() const
{
    return _dPtr ? _dPtr->depth: 0;
}

int Image::bytesPerLine() const
{
    return _dPtr ? _dPtr->bytesPerLine : 0;
}

Image::Format Image::format() const
{
    return _dPtr ? _dPtr->format : Format::Format_Invalid;
}

unsigned char *Image::bytes()
{
    // FIXME: I will eventually need to return a copy instead of actual data for dealing with sync issues, 
    // as this is shared between Image and DrawEngine
    return _dPtr ? _dPtr->data : nullptr;
}

DrawEngine *Image::drawEngine() const
{
    if (!_dPtr) {
        return nullptr;
    }
    
    if (!_dPtr->drawEngine) {
        Drawable *drawable = const_cast<Image *>(this);
        _dPtr->drawEngine = new RasterDrawEngine(drawable);
    }
    
    return _dPtr->drawEngine;
}

bool Image::isNull() const
{
    return !_dPtr;
}

ImageDataPrivate::ImageDataPrivate() :
    width(0), height(0), depth(0), bytesPerLine(0), totalBytes(0),
    data(nullptr), isDataOwner(true),
    format(Image::Format_RGB16),
    drawEngine(nullptr)
{
}

ImageDataPrivate::ImageDataPrivate(int width, int height, int depth, Image::Format format, 
                                   std::size_t bytesPerLine, std::size_t totalBytes) :
    width(width), height(height), depth(depth), 
    bytesPerLine(bytesPerLine), totalBytes(totalBytes), format(format),
    drawEngine(nullptr)
{
}

ImageDataPrivate::~ImageDataPrivate()
{
    if (data && isDataOwner) {
        free(data);
    }
    
    delete drawEngine;
}

ImageDataPrivate *ImageDataPrivate::create(const Size &size, Image::Format format)
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
    
    auto imageData = 
        new ImageDataPrivate(width, height, depth, format, imageParams.bytesPerLine, imageParams.totalBytes);
    imageData->data = (unsigned char *)malloc(imageData->totalBytes);
    if (!imageData->data) {
        return nullptr;
    }
    
    return imageData;
}

ImageDataPrivate *ImageDataPrivate::create(unsigned char *data, int width, int height, int bytesPerLine, 
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
    
    auto imageData = new ImageDataPrivate(width, height, depth, format, bytesPerLine, totalBytes);    
    imageData->data = data;
    imageData->isDataOwner = false;
    
    return imageData;
}
    
ImageDataPrivate::ImageSizeParameters 
ImageDataPrivate::calculateImageParameters(int width, int height, int depth)
{
    // FIXME: this multiplication and the one on totalBytes could potentially overflow
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
