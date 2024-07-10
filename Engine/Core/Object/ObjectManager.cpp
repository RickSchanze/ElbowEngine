/**
 * @file ObjectManager.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "ObjectManager.h"
#include "CoreGlobal.h"
#include "Object.h"

bool ObjectManager::AddObject(Object* new_object)
{
    if (!IsValid(new_object)) return false;
    if (IsIDValid(new_object->GetID()))
    {
        objects_[new_object->GetID()] = new_object;
        return true;
    }

    const auto ValidID = GetNextValidID();
    ObjectCreateHelper::SetObjectID(new_object, ValidID);
    objects_[ValidID] = new_object;
    return true;
}

bool ObjectManager::IsIDValid(const uint32_t id) const
{
    return id != 0 && !objects_.contains(id);
}

int32_t ObjectManager::GetNextValidID()
{
    while (!IsIDValid(id_count_++))
    {
    }
    return id_count_;
}

bool ObjectManager::RemoveObject(const uint32_t id)
{
    if (objects_.erase(id) != 0)
    {
        return true;
    }
    return false;
}
ObjectManager::~ObjectManager()
{
    while (!objects_.empty())
    {
        delete objects_.begin()->second;
    }
    objects_.clear();
}
