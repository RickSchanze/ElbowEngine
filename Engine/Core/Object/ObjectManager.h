/**
 * @file ObjectManager.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "Singleton/Singleton.h"

class Object;
class ObjectManager : public Singleton<ObjectManager>
{
public:
    /**
     * 向对象管理器中添加一个对象
     * @param new_object 对象
     * @return
     */
    bool AddObject(Object* new_object);

    /**
     * 此ID是否可用
     * @param id
     * @return
     */
    bool IsIDValid(uint32_t id) const;

    /**
     * 获取下一个有效的ID
     * @return
     */
    int32_t GetNextValidID();

    /**
     * 移除ID对应的对象,此函数不会调用delete
     * @param id
     * @return 不存在则返回False
     */
    bool RemoveObject(uint32_t id);

    // 根据ID获取一个Object，如果不存在则返回nullptr
    Object* GetObjectById(const uint32_t id)
    {
        if (objects_.contains(id))
        {
            return objects_[id];
        }
        return nullptr;
    }

    template<typename T>
        requires std::derived_from<T, Object>
    T* GetObjectById(const uint32_t id)
    {
        return dynamic_cast<T*>(GetObjectById(id));
    }

    int32_t GetObjectCount() const { return objects_.size(); }

    ~ObjectManager() override;

private:
    THashMap<uint32_t, Object*> objects_;
    int32_t                     id_count_ = 1;
};
