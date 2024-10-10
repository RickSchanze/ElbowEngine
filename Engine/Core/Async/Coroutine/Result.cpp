/**
 * @file Result.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Result.h"

void async::coro::Result<void>::Get() const
{
    if (exception_ptr_)
    {
        std::rethrow_exception(exception_ptr_);
    }
}
