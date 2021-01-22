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
    mScreenImage = new Image(); // TODO: recreate for the new size/format
}

void FbScreen::redraw()
{

}
