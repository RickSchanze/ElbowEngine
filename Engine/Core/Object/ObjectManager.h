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
class ObjectManager : public Singleton<ObjectManager> {
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
    bool IsIDValid(UInt32 id) const;

    /**
     * 获取下一个有效的ID
     * @return
     */
    UInt32 GetNextValidID();

    /**
     * 移除ID对应的对象,此函数不会调用delete
     * @param id
     * @return 不存在则返回False
     */
    bool RemoveObject(UInt32 id);

    // 根据ID获取一个Object，如果不存在则返回nullptr
    Object* GetObjectById(const UInt32 id) {
        if (objects_.contains(id)) {
            return objects_[id];
        }
        return nullptr;
    }

    template<typename T>
        requires std::derived_from<T, Object>
    T* GetObjectById(const UInt32 id) {
        return dynamic_cast<T*>(GetObjectById(id));
    }

    UInt32 GetObjectCount()const { return objects_.size(); }

    ~ObjectManager();

private:
    THashMap<UInt32, Object*> objects_;
    UInt32                    id_count_ = 1;
};
