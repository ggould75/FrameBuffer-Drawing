#include "FbScreen.hpp"
#include "Painter.hpp"
#include "Image.hpp"
#include "Point.hpp"

FbScreen::FbScreen() : mFormat(Image::Format_RGB32), _mPainter(nullptr)
{

}

FbScreen::~FbScreen()
{
    delete _mPainter;
}

bool FbScreen::initialize() 
{
    return true;
}

void FbScreen::setGeometry(const Rect &rect)
{
    delete _mPainter;
    _mPainter = nullptr;
    mGeometry = rect;
    mScreenImage = Image(rect.size(), mFormat);
}

void FbScreen::initializeCompositor()
{
    mScreenImage = Image(mGeometry.size(), mFormat);
    // TODO: schedule update event?
}

void FbScreen::redraw()
{
    // TODO
    // Foreach region to redraw
    //    Foreach window on screen that intersect the region
    //       draw interecting window content
    // Draw cursor
    //
    // - All painting should happen on mScreenImage.
    // - This method should eventually return a region or rect
    // - Eventually I will also need locks (mutex) when writing/reading an Image!
}

void FbScreen::redraw(Image *image)
{
    if (!_mPainter) {
        _mPainter = new Painter(&mScreenImage);
        _mPainter->begin();
    }
    
    // Simply draw my beautiful art painting at screen origin for now
    _mPainter->drawImage(Point(0, 0), image);
}
