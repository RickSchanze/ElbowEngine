/**
 * @file Image.cpp
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#include "Image.h"

platform::rhi::ImageDesc platform::rhi::ImageDesc::Default()
{
    return ImageDesc{"Invalid", 0, 0, IU_Max, Format::Count};
}
