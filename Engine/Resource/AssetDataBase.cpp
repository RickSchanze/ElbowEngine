//
// Created by Echo on 2025/3/23.
//
#include "AssetDataBase.hpp"
#include "Assets/Font/Font.hpp"
#include "Assets/Font/FontMeta.hpp"
#include "Assets/Material/Material.hpp"
#include "Assets/Material/MaterialMeta.hpp"
#include "Assets/Mesh/Mesh.hpp"
#include "Assets/Mesh/MeshMeta.hpp"
#include "Assets/Shader/Shader.hpp"
#include "Assets/Shader/ShaderMeta.hpp"
#include "Assets/Texture/Texture2D.hpp"
#include "Assets/Texture/Texture2DMeta.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Folder.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Core/Serialization/XMLArchive.hpp"
#include "Project.hpp"
#include "SQLiteCpp/SQLiteCpp.h"

#include <fstream>

using namespace exec;

Object* internal::GetObjectByHandle(const ObjectHandle handle)
{
    return ObjectManager::GetObjectByHandle(handle);
}

void AssetDataBase::Startup()
{
    const auto& proj = Project::GetCurrentProject();
    const auto db_path = proj.GetDatabasePath();
    if (!Path::IsExist(db_path))
    {
        Folder::CreateFolder(db_path);
    }
    Assert(Path::IsFolder(db_path), "DataBasePath in project must be a valid folder path.");
    const auto db_file = Path::Combine(db_path, "AssetDataBase.db");
    db_ = New<SQLite::Database>(db_file.Data(), SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    SQLHelper::InitializeDataBase(*db_);
    CreateAssetTables();
}

void AssetDataBase::Shutdown()
{
    Delete(db_);
    db_ = nullptr;
}

template <typename T, typename TMeta>
ExecFuture<ObjectHandle> InternalImport(const StringView InQuery, StringView InPath, ObjectRegistry& InRegistry)
{
    auto Result = AssetDataBase::QueryMeta<TMeta>(InQuery);
    if (Result)
    {
        auto& Meta = *Result;
        ObjectHandle Handle = Meta.ObjectHandle;
        auto* Obj = InRegistry.GetObjectByHandle(Handle);
        if (Obj != nullptr)
        {
            InRegistry.RemoveObject(Obj);
        }
        auto* Asset = ObjectManager::CreateNewObjectAsync<T>().Get();
        Asset->InternalSetAssetHandle(Handle);
        return Asset->PerformPersistentObjectLoadAsync();
    }
    else
    {
        TMeta NewMeta = {};
        NewMeta.Path = InPath;
        auto* Asset = ObjectManager::CreateNewObjectAsync<T>().Get();
        NewMeta.ObjectHandle = InRegistry.NextPersistentHandle().Get();
        if constexpr (Traits::SameAs<T, Texture2D>)
        {
            if (InPath.EndsWith(".exr") || InPath.EndsWith(".hdr"))
            {
                NewMeta.Format = NRHI::Format::R32G32B32A32_Float;
            }
        }
        AssetDataBase::InsertMeta(NewMeta);
        Asset->InternalSetAssetHandle(NewMeta.ObjectHandle);
        return Asset->PerformPersistentObjectLoadAsync();
    }
}

ExecFuture<ObjectHandle> AssetDataBase::Import(const StringView path)
{
    ProfileScope _(__func__);
    Assert(IsMainThread(), "Import只能在主线程调用");
    // 先查一下是否存在, 存在的话按现有配置重新导入
    auto query = String::Format("path = '{}'", *path);
    auto& registry = ObjectManager::GetRegistry();
    if (path.EndsWith(".fbx") || path.EndsWith(".obj"))
    {
        return InternalImport<Mesh, MeshMeta>(query, path, registry);
    }
    if (path.EndsWith(".slang"))
    {
        return InternalImport<Shader, ShaderMeta>(query, path, registry);
    }
    if (path.EndsWith(".png") || path.EndsWith(".exr") || path.EndsWith(".hdr") || path.EndsWith(".jpg"))
    {
        return InternalImport<Texture2D, Texture2DMeta>(query, path, registry);
    }
    if (path.EndsWith(".ttf"))
    {
        return InternalImport<Font, FontMeta>(query, path, registry);
    }
    return exec::MakeExecFuture(0);
}

Object* AssetDataBase::LoadFromPath(const StringView path)
{
    ProfileScope _(__func__);
    const ObjectHandle obj_handle = LoadFromPathAsync(path).Get();
    return ObjectManager::GetObjectByHandle(obj_handle);
}

template <typename T, typename TMeta>
ExecFuture<ObjectHandle> InternalLoadAsync(const StringView path)
{
    if (const auto meta_op = AssetDataBase::QueryMeta<TMeta>(String::Format("path = '{}'", *path)); !meta_op)
    {
        Log(Error) << String::Format("资产{}未在资产数据库中找到.", *path);
        return exec::MakeExecFuture(0);
    }
    else
    {
        auto& meta = *meta_op;
        ObjectHandle handle = meta.ObjectHandle;
        ObjectRegistry& registry = ObjectManager::GetRegistry();
        Object* obj = registry.GetObjectByHandle(handle);
        if (obj != nullptr)
        {
            return MakeExecFuture(handle);
        }
        else
        {
            T* asset = ObjectManager::CreateNewObjectAsync<T>().Get();
            asset->InternalSetAssetHandle(handle);
            return asset->PerformPersistentObjectLoadAsync();
        }
    }
}

ExecFuture<ObjectHandle> AssetDataBase::LoadFromPathAsync(StringView InPath)
{
    ProfileScope _(__func__);
    if (InPath.EndsWith(".slang"))
    {
        return InternalLoadAsync<Shader, ShaderMeta>(InPath);
    }
    if (InPath.EndsWith(".fbx") || InPath.EndsWith(".obj"))
    {
        return InternalLoadAsync<Mesh, MeshMeta>(InPath);
    }
    if (InPath.EndsWith(".png") || InPath.EndsWith(".exr") || InPath.EndsWith(".hdr") || InPath.EndsWith(".jpg"))
    {
        return InternalLoadAsync<Texture2D, Texture2DMeta>(InPath);
    }
    if (InPath.EndsWith(".ttf"))
    {
        return InternalLoadAsync<Font, FontMeta>(InPath);
    }
    if (InPath.EndsWith(".mat"))
    {
        // 查看资产数据库是否存在
        if (Optional<MaterialMeta> meta_op = QueryMeta<MaterialMeta>(String::Format("path = '{}'", InPath)))
        {
            const MaterialMeta& meta = *meta_op;
            ObjectHandle DatabaseHandle = meta.ObjectHandle;
            ObjectRegistry& registry = ObjectManager::GetRegistry();
            if (const Object* obj = registry.GetObjectByHandle(DatabaseHandle); obj != nullptr)
            {
                return MakeExecFuture(DatabaseHandle);
            }
            else
            {
                // TODO: 这里是叫Async，所以不应该直接Get同步等待
                // 创建一个instanced asset作为临时载入, 此时它被注册入registry
                Material* MaterialAsset = ObjectManager::CreateNewObjectAsync<Material>().Get();
                std::ifstream InputStream(*InPath);
                if (!InputStream)
                {
                    VLOG_ERROR("加载资产", *InPath, "失败: errno=", errno, ", strerror=", strerror(errno));
                    Destroy(MaterialAsset);
                    return MakeExecFuture(0);
                }
                XMLInputArchive InputArchive(InputStream);
                const auto OldInstanceHandle = MaterialAsset->GetHandle();
                // 序列化一个object 如果成功那么这个persistent handle也会被入册到registry
                // 此时registry有两个一样的对象, 一是之前作为临时instanced object注册的, 在一个就是序列化时注册
                InputArchive.Deserialize(*MaterialAsset);
                const auto FileHandle = MaterialAsset->GetHandle();
                Assert(FileHandle == DatabaseHandle, "FileHandle != DatabaseHandle");
                // 校验通过则将临时注册的取消注册
                // 这里没有手动注册新的ObjectHandle是因为它已经在Object的Serialization_AfterLoad完成
                ObjectManager::GetRegistry().UnregisterHandle(OldInstanceHandle);
                auto Result = MaterialAsset->PerformPersistentObjectLoadAsync();
                return Result;
            }
        }
    }
    return MakeExecFuture(0);
}

template <typename T>
static String QueryPath(ObjectHandle handle)
{
    if constexpr (std::is_same_v<T, Shader>)
    {
        auto meta_op = AssetDataBase::QueryMeta<ShaderMeta>(handle);
        if (!meta_op)
        {
            Log(Warn) << String::Format("资产{}未在资产数据库中找到.", handle);
            return "";
        }
        auto& meta = *meta_op;
        return meta.Path;
    }
    return "";
}

ExecFuture<ObjectHandle> AssetDataBase::LoadAsync(ObjectHandle handle, const Type* asset_type)
{
    if (asset_type == nullptr)
        return MakeExecFuture(0);
    if (ObjectManager::IsObjectExist(handle))
    {
        return MakeExecFuture(handle);
    }
    String path;
    if (asset_type == TypeOf<Shader>())
    {
        path = QueryPath<Shader>(handle);
    }
    if (path.IsEmpty())
        return MakeExecFuture(0);
    return LoadFromPathAsync(path);
}

ExecFuture<ObjectHandle> AssetDataBase::LoadOrImportAsync(const StringView path)
{
    if (path.EndsWith(".slang"))
    {
        if (const auto op_meta = QueryMeta<ShaderMeta>(String::Format("Path = '{}'", *path)))
        {
            return LoadFromPathAsync(path);
        }
        return Import(path);
    }
    if (path.EndsWith(".exr") || path.EndsWith(".png") || path.EndsWith(".hdr") || path.EndsWith(".jpg"))
    {
        if (const auto op_meta = QueryMeta<Texture2DMeta>(String::Format("Path = '{}'", *path)))
        {
            return LoadFromPathAsync(path);
        }
        return Import(path);
    }
    if (path.EndsWith(".fbx") || path.EndsWith(".obj"))
    {
        if (const auto op_meta = QueryMeta<MeshMeta>(String::Format("Path = '{}'", *path)))
        {
            return LoadFromPathAsync(path);
        }
        return Import(path);
    }
    return MakeExecFuture(0);
}

Object* AssetDataBase::LoadOrImport(const StringView path)
{
    return ObjectManager::GetObjectByHandle(LoadOrImportAsync(path).Get());
}

#define CREATE_ASSET_TABLE(asset_type)                                                                                                               \
    {                                                                                                                                                \
        const Type* type = TypeOf<asset_type>();                                                                                                     \
        tables_[type] = std::move(SQLHelper::CreateTable(*db_, type));                                                                               \
    }

bool AssetDataBase::CreateAsset(Asset* InAsset, StringView InPath)
{
    if (InAsset == nullptr)
    {
        Log(Error) << "传入的资产为空";
        return false;
    }
    const Type* type = InAsset->GetType();
    if (type == TypeOf<Mesh>() || type == TypeOf<Shader>())
    {
        Log(Error) << String::Format("{}只支持导入而不支持创建.", type->GetName());
        return false;
    }
    if (type == TypeOf<Material>())
    {
        if (auto exist_meta = QueryMeta<MaterialMeta>(String::Format("path = '{}'", InPath)))
        {
            Log(Error) << String::Format("{}已存在", InPath);
            return false;
        }
        ObjectHandle handle = ObjectManager::GetRegistry().NextPersistentHandle().Get();
        InAsset->InternalSetAssetHandle(handle);
        std::ofstream OutputStream(*InPath);
        if (!OutputStream)
        {
            VLOG_ERROR("创建资产", *InPath, "失败: errno=", errno, ", strerror=", strerror(errno));
            return false;
        }
        XMLOutputArchive OutputArchive(OutputStream);
        OutputArchive.Serialize(*InAsset);
        MaterialMeta meta;
        meta.ObjectHandle = handle;
        meta.Path = InPath;
        InsertMeta(meta);
        return true;
    }
    if (type == TypeOf<Texture2D>())
    {
        if (auto exist_meta = QueryMeta<Texture2DMeta>(String::Format("path = '{}'", *InPath)))
        {
            Log(Error) << String::Format("{}已存在", InPath);
            return false;
        }
        ObjectHandle handle = ObjectManager::GetRegistry().NextPersistentHandle().Get();
        InAsset->InternalSetAssetHandle(handle);
        auto tex = static_cast<Texture2D*>(InAsset);
        Assert(StringView(tex->GetAssetPath()) == InPath, "创建纹理资源失败: 路径不匹配");
        Texture2DMeta meta;
        meta.ObjectHandle = handle;
        meta.Path = InPath;
        meta.IsDynamic = false;
        meta.Height = tex->GetHeight();
        meta.Width = tex->GetWidth();
        meta.Format = tex->GetFormat();
        meta.SpritesString = tex->GetSpriteRangeString();
        InsertMeta(meta);
        return true;
    }
    return false;
}

void AssetDataBase::CreateAssetTables()
{
    CREATE_ASSET_TABLE(MeshMeta);
    CREATE_ASSET_TABLE(ShaderMeta);
    CREATE_ASSET_TABLE(Texture2DMeta);
    CREATE_ASSET_TABLE(MaterialMeta);
    CREATE_ASSET_TABLE(FontMeta);
}
