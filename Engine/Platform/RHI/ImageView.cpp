/**
 * @file ImageView.cpp
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#include "ImageView.h"

#include "Image.h"

platform::rhi::ImageView::~ImageView()
{
    if (desc_.image)
    {
        desc_.image->RemoveView(this);
    }
}
