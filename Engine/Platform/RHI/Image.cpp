/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#include "Image.h"
void platform::rhi::Image::AddView(ImageView* new_view)
{
    views_.push_back(new_view);
}

void platform::rhi::Image::RemoveView(ImageView* view)
{
    std::erase(views_, view);
}
