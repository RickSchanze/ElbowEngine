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
        VLOG_ERROR("读取场景文件[", *mMeta.Path, "]失败, 此文件存在");
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
}

// 将树状结构扁平化, 并且把里面所有的ObjectHandle都转化为Persistent的
static void FlattenAndTransferObjectToPersistent(const ObjectHandle InObjectHandle, Array<Object*>& OutFlattenObjects)
{
    if (InObjectHandle == 0)
        return;
    Object* MyObject = ObjectManager::GetObjectByHandle(InObjectHandle);
    if (!MyObject)
    {
        if (InObjectHandle > 0)
        {
            // 失效视为被删除了
            ObjectManager::RecycleHandleAsync(InObjectHandle);
        }
        // else分支代表创建了但是没保存然后又删除了
        return;
    }
    if (InObjectHandle < 0)
    {
        MyObject->SetObjectHandle(ObjectManager::GetNextPersistentHandleAsync().Get());
    }
    if (!MyObject->IsFlagSet(OFB_Persistent))
    {
        MyObject->SetFlag(OFB_Persistent);
    }
    OutFlattenObjects.Add(MyObject);
    const Type* ObjectType = MyObject->GetType();
    auto ObjectFields = ObjectType->GetFields();
    for (Int32 Index = 0; Index < ObjectFields.Count(); Index++)
    {
        const Field* ObjectField = ObjectFields[Index];
        if (ObjectField->IsDefined(Field::Transient))
            continue;
        if (ObjectField->GetType() == ObjectPtrBase::GetStaticType())
        {
            ObjectPtrBase ObjectPtr = NReflHelper::GetValueDirectly<ObjectPtrBase>(ObjectField, MyObject);
            FlattenAndTransferObjectToPersistent(ObjectPtr.GetHandle(), OutFlattenObjects);
        }
    }
}

void Scene::Serialization_Save(OutputArchive& Archive) const
{
    CPU_PROFILING_SCOPE;
    // 1 Pass 扁平化树, 找出所有需要序列化的Object
    Array<Object*> ObjectsToSerialize;
    for (Int32 Index = 0; Index < mTopObjects.Count(); Index++)
    {
        FlattenAndTransferObjectToPersistent(mTopObjects[Index].GetHandle(), ObjectsToSerialize);
    }
    // 2 Pass 序列化这些对象
    for (auto ObjectToSerialize : ObjectsToSerialize)
    {
        // 写入Meta
        Archive.SetNextScopeName("TypeMeta");
        Archive.BeginScope();
        Archive.WriteType("TypeName", static_cast<String>(ObjectToSerialize->GetType()->GetName()));
        Archive.WriteType("TypeHash", ObjectToSerialize->GetType()->GetHashCode());
        Archive.EndScope();
        Archive.SetNextScopeName("Data");
        Archive.BeginScope();
        ObjectToSerialize->Serialization_Save(Archive);
        Archive.EndScope();
    }
    // 3 Pass 构建这些对象的关系
    Assert(Evt_SceneSerializeObjectTree.HasBound(), "请绑定序列化树的方法");
    Archive.SetNextScopeName("Tree");
    Archive.BeginScope();
    for (Object* ObjectToSerialize : ObjectsToSerialize)
    {
        Evt_SceneSerializeObjectTree.Invoke(Archive, ObjectToSerialize);
    }
    Archive.EndScope();
}
