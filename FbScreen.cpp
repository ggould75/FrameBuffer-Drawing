#include "FbScreen.hpp"
#include "Painter.hpp"
#include "Image.hpp"

FbScreen::FbScreen()
{

}

FbScreen::~FbScreen()
{

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
    mScreenImage = Image(); // TODO: pass size/format
}

void FbScreen::initializeCompositor()
{
    mScreenImage = Image(); // TODO: pass size/format
    // TODO: schedule update event? Do I need this?
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
}
