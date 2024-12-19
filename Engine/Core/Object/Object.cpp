/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"
#include "Core/Core.h"
#include "Core/Log/Logger.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/Archive.h"

#include "Core.Object.generated.h"

GENERATED_SOURCE()

void core::Object::AddReferencing(ObjectHandle handle)
{
    referencing_.push_back(handle);
}

void core::Object::RemoveReferencing(ObjectHandle handle)
{
    erase(referencing_, handle);
}

void core::Object::AddReferenced(ObjectHandle handle)
{
    referenced_.push_back(handle);
}

void core::Object::RemoveReferenced(ObjectHandle handle)
{
    erase(referenced_, handle);
}

void core::Object::PostSerialized() {}

void core::Object::PostDeserialized() {}

namespace core
{

}   // namespace core
