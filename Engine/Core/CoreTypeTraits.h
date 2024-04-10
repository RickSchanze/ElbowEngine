/**
 * @file CoreTypeTraits.h
 * @author Echo 
 * @Date 2024/4/11
 * @brief 
 */

#ifndef CORETYPETRAITS_H
#define CORETYPETRAITS_H

#include <type_traits>
// 数字类型
template <typename T>
concept IsNumeric = std::is_arithmetic_v<T>;

// 是个布尔类型
template <typename T>
concept IsBoolean = std::is_same_v<T, bool>;


#endif //CORETYPETRAITS_H
