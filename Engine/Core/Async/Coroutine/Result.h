/**
 * @file Result.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once
#include <exception>
#include "CoroutineCommon.h"

namespace async::coro
{
template<>
struct Result<void>
{
    explicit Result() = default;
    explicit Result(const std::exception_ptr& exception_ptr) : exception_ptr_(exception_ptr) {}

    /**
     * 获取Result的值，如果有异常则会抛出
     */
    void Get() const;

private:
    std::exception_ptr exception_ptr_;
};
}   // namespace async::coro
