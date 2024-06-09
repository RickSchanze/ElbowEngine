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
     * @param NewObject 对象
     * @return
     */
    bool AddObject(Object* NewObject);

    /**
     * 此ID是否可用
     * @param ID
     * @return
     */
    [[nodiscard]] bool IsIDValid(UInt32 ID) const;

    /**
     * 获取下一个有效的ID
     * @return
     */
    UInt32 GetNextValidID();

    /**
     * 移除ID对应的对象,此函数不会调用delete
     * @param ID
     * @return 不存在则返回False
     */
    bool RemoveObject(UInt32 ID);

    // 根据ID获取一个Object，如果不存在则返回nullptr
    Object* GetObjectById(UInt32 ID) {
        if (mObjects.contains(ID)) {
            return mObjects[ID];
        }
        return nullptr;
    }

    template<typename T>
        requires std::derived_from<T, Object>
    T* GetObjectById(UInt32 ID) {
        return dynamic_cast<T*>(GetObjectById(ID));
    }

    UInt32 GetObjectCount() { return mObjects.size(); }

    ~ObjectManager();

private:
    THashMap<UInt32, Object*> mObjects;
    UInt32                    mIDCount = 1;
};
