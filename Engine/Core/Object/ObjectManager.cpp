//
// Created by Echo on 2025/3/22.
//

#include "ObjectManager.hpp"

#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Async/Exec/Just.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Folder.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Memory/New.hpp"
#include "Core/Profile.hpp"
#include "Core/Serialization/XMLArchive.hpp"
#include "Object.hpp"
#include "PersistentObject.hpp"

#include <fstream>

NExec::ExecFuture<ObjectHandle> ObjectRegistry::NextPersistentHandle()
{
    CPU_PROFILING_SCOPE;
    auto GetNext = [this]() -> ObjectHandle
    {
        if (!mFreeHandles.Empty())
        {
            const auto handle = mFreeHandles.Last();
            mFreeHandles.RemoveLast();
            return handle;
        }
        return mNextPersistentHandle++;
    };
    if (IsMainThread())
    {
        return NExec::MakeExecFuture(GetNext());
    }

    auto task = NExec::Just() | NExec::Then([GetNext]() { return GetNext(); });
    return ThreadManager::ScheduleFutureAsync(task, NamedThread::Game, true);
}

Object* ObjectRegistry::GetObjectByHandle(ObjectHandle handle)
{
    CPU_PROFILING_SCOPE;
    if (handle == 0)
        return nullptr;
    if (!mObjects.Contains(handle))
        return nullptr;
    Object* ptr = mObjects[handle];
    if (ptr->IsPendingKill())
        return nullptr;
    return ptr;
}

void ObjectRegistry::RemoveObject(Object* object)
{
    if (object == nullptr)
        return;
    const ObjectHandle handle = object->GetHandle();
    if (handle == 0)
    {
        Log(Error) << "ObjectHandle为0, 无法删除";
        return;
    }
    if (object->IsPersistent())
    {
        if (static_cast<PersistentObject*>(object)->IsLoaded())
        {
            static_cast<PersistentObject*>(object)->PerformUnload();
        }
    }
    Delete(object);
    mObjects.Remove(handle);
}

void ObjectRegistry::RemoveObject(ObjectHandle InHandle)
{
    if (InHandle == 0)
    {
        Log(Error) << "ObjectHandle为0, 无法删除";
        return;
    }
    Object* Obj = GetObjectByHandle(InHandle);
    if (!Obj)
    {
        VLOG_ERROR("ObjectHandle为", InHandle, "的对象已经失效");
        return;
    }
    if (Obj->IsPersistent())
    {
        if (static_cast<PersistentObject*>(Obj)->IsLoaded())
        {
            static_cast<PersistentObject*>(Obj)->PerformUnload();
        }
    }
    Delete(Obj);
    mObjects.Remove(InHandle);
}

void ObjectRegistry::RemoveAllObjects()
{
    for (Int32 i = 0; i < 10; i++)
    {
        RemoveAllObjectLayered();
    }
    while (!mObjects.Empty())
    {
        RemoveObject(mObjects.begin()->second);
    }
}

void ObjectRegistry::RemoveAllObjectLayered()
{
    Int32 skip = 0;
    while (!mObjects.Empty())
    {
        auto begin = mObjects.begin();
        std::advance(begin, skip);
        if (begin == mObjects.end())
        {
            return;
        }
        Object* obj = begin->second;
        if (obj->mReferenced.Empty())
        {
            RemoveObject(obj);
        }
        else
        {
            skip++;
        }
    }
}

void ObjectRegistry::RegisterObject(Object* object)
{
    CPU_PROFILING_SCOPE;
    std::lock_guard Lock(mMutex);
    if (object == nullptr)
        return;
    Assert(!mObjects.Contains(object->GetHandle()), "ObjectHandle的重复注册");
    mObjects[object->GetHandle()] = object;
}

void ObjectRegistry::UnregisterHandle(ObjectHandle handle)
{
    CPU_PROFILING_SCOPE;
    std::lock_guard Lock(mMutex);
    if (handle == 0)
        return;
    if (!mObjects.Contains(handle))
        return;
    mObjects.Remove(handle);
}

void ObjectRegistry::Save() const
{
#if WITH_EDITOR
    std::ofstream FileStream(REGISTRY_PATH);
    XMLOutputArchive OutputArchive(FileStream);
    OutputArchive.Serialize(*this);
#endif
}

NExec::ExecFuture<> ObjectRegistry::RecycleHandleAsync(ObjectHandle InHandle)
{
    Assert(!mObjects.Contains(InHandle), "仍存在于mObjects的Handle不能被回收");
    Assert(InHandle > 0, "Instanced Object的Handle or 无效的Handle不能被回收");
    return ThreadManager::ScheduleFutureAsync(                  //
        NExec::Just() | NExec::Then([this, InHandle]() {  //
            mFreeHandles.Add(InHandle);
        }),
        NamedThread::Game, true//
    );
}

void ObjectManager::Startup()
{
    if (!File::IsExist(REGISTRY_PATH))
    {
        Folder::CreateFolder(Path::GetParent(REGISTRY_PATH));
        std::ofstream FileStream(REGISTRY_PATH);
        XMLOutputArchive OutputArchive(FileStream);
        OutputArchive.Serialize(mRegistry);
    }
    else
    {
        std::ifstream InputStream(REGISTRY_PATH);
        XMLInputArchive InputArchive(InputStream);
        InputArchive.Deserialize(mRegistry);
    }
}

void ObjectManager::Shutdown()
{
#if WITH_EDITOR
    SaveObjectRegistry();
#endif
    mRegistry.RemoveAllObjects();
}
