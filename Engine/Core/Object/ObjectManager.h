/**
 * @file ObjectManager.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H
#include "Core/CoreDef.h"
#include "Core/Singleton/Singleton.h"

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
    [[nodiscard]] bool IsIDValid(uint32 ID) const;

    /**
     * 获取下一个有效的ID
     * @return
     */
    uint32 GetNextValidID();

    /**
     * 移除ID对应的对象,此函数不会调用delete
     * @param ID
     * @return 不存在则返回False
     */
    bool RemoveObject(uint32 ID);

private:
    HashMap<uint32, Object*> mObjects;
    int32                    mIDCount = 0;
};

#endif //OBJECTMANAGER_H
