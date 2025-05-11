//
// Created by kita on 25-5-11.
//

#include "Scene.hpp"

#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Core/Serialization/XMLArchive.hpp"
#include "Resource/AssetDataBase.hpp"

#include <fstream>

void Scene::PerformLoad()
{
    // 由于场景文件的特殊性, 场景文件PerformLoad在PerformLoad里读取文件并创建一系列场景对象
    const Optional<SceneMeta> OpMeta = AssetDataBase::QueryMeta<SceneMeta>(mHandle);
    if (!OpMeta)
    {
        VLOG_ERROR("读取场景文件元数据失败, Handle=", mHandle);
        return;
    }
    mMeta = *OpMeta;
    if (!File::IsExist(mMeta.Path))
    {
        VLOG_ERROR("读取场景文件[", mMeta.Path, "]失败, 此文件存在");
        return;
    }
    std::ifstream InputStream(*mMeta.Path);
    XMLInputArchive InputArchive(InputStream);
    Serialization_Load(InputArchive);
}

void Scene::PerformUnload()
{
    for (auto TopObject : mTopObjects)
    {
        Destroy(TopObject);
    }
}

void Scene::Save()
{
    std::ofstream OutputStream(*mMeta.Path);
    XMLOutputArchive OutputArchive(OutputStream);
    Serialization_Save(OutputArchive);
}

void Scene::Serialization_Load(InputArchive& Archive)
{
}

static void SerializeSceneObject(OutputArchive& Archive, Object* InObj)
{
    if (InObj->GetHandle() < 0)
    {
        const ObjectHandle NextHandle = ObjectManager::GetRegistry().NextPersistentHandle().Get();
        InObj->SetObjectHandle(NextHandle);
    }
    if (InObj->IsFlagSet(OFB_Persistent))
    {
        InObj->SetFlag(OFB_Persistent);
    }
}

void Scene::Serialization_Save(OutputArchive& Archive) const
{
    Archive.WriteArraySize(mTopObjects.Count());
    for (Int32 Index = 0; Index < mTopObjects.Count(); Index++)
    {
        Object* Obj = mTopObjects[Index];
        if (!Obj)
        {
            VLOG_ERROR("无效对象, ObjectHandle=", mTopObjects[Index].GetHandle());
            continue;
        }
        const Type* ObjType = Obj->GetType();
        Archive.SetNextScopeName("Type");
        Archive.BeginScope();
        Archive.WriteString("Name", ObjType->GetName());
        Archive.WriteType("Hash", ObjType->GetHashCode());
        Archive.EndScope();
        Archive.SetNextScopeName("Data");
        Archive.BeginScope();
        SerializeSceneObject(Archive, Obj);
        Archive.EndScope();
    }
}
