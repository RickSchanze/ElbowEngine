/**
 * @file Awaiter.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Awaiter.h"


namespace async::coro
{

bool AwaiterBase::CanAwake()
{
    NEVER_ENTRY_WARNING()
    return false;
}

void AwaiterBase::Awake()
{
    NEVER_ENTRY_WARNING()
}

AwaiterBase::AwaiterBase()
{
    id = s_id_counter++;
}

}   // namespace async::coro
