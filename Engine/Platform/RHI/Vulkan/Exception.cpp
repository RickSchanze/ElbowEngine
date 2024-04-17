/**
 * @file Exception.cpp
 * @author Echo 
 * @Date 24-4-18
 * @brief 
 */

#include "Exception.h"

#include <format>
String VulkanException::What() const {
    return std::format(L"VulkanException: {}", mMessage);
}
