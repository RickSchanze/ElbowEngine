/**
 * @file ObjectManager.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "ObjectManager.h"

#include "CoreEvents.h"
#include "CoreGlobal.h"
#include "Object.h"
#include "Utils/ContainerUtils.h"

ObjectManager::ObjectManager()
{
    OnAppExit.Add(&ObjectManager::DestroyAllObjects);
}

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

void ObjectManager::DestroyAllObjects()
{
    auto m = Get();
    if (m == nullptr)
    {
        return;
    }
    while (!m->objects_.empty())
    {
        auto obj_to_del = m->objects_.begin()->second;
        // 组件由对象自己释放
        if (!obj_to_del->IsComponent())
        {
            Delete(m->objects_.begin()->second);
        }
        else
        {
            m->objects_.erase(m->objects_.begin());
        }
    }
    m->objects_.clear();
}

ObjectManager::~ObjectManager()
{
    DestroyAllObjects();
}
