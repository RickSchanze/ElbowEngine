//
// Created by Echo on 2025/3/23.
//
#include "AssetDataBase.hpp"

#include <tinyexr.h>

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
#include "Core/Serialization/YamlArchive.hpp"
#include "Project.hpp"
#include "SQLiteCpp/SQLiteCpp.h"

using namespace exec;

Object *internal::GetObjectByHandle(ObjectHandle handle) { return ObjectManager::Get()->GetObjectByHandle(handle); }

void AssetDataBase::Startup() {
    const auto &proj = Project::GetCurrentProject();
    const auto db_path = proj.GetDatabasePath();
    if (!Path::IsExist(db_path)) {
        Folder::CreateFolder(db_path);
    }
    Assert(Path::IsFolder(db_path), "DataBasePath in project must be a valid folder path.");
    const auto db_file = Path::Combine(db_path, "AssetDataBase.db");
    db_ = New<SQLite::Database>(db_file.Data(), SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    SQLHelper::InitializeDataBase(*db_);
    CreateAssetTables();
}

void AssetDataBase::Shutdown() {
    Delete(db_);
    db_ = nullptr;
}

template<typename T, typename TMeta>
exec::ExecFuture<ObjectHandle> InternalImport(StringView query, StringView path, ObjectRegistry &registry) {
    auto result = AssetDataBase::QueryMeta<TMeta>(query);
    if (result) {
        auto &meta = *result;
        ObjectHandle handle = meta.object_handle;
        auto *obj = registry.GetObjectByHandle(handle);
        if (obj != nullptr) {
            registry.RemoveObject(obj);
        }
        auto *asset = ObjectManager::CreateNewObjectAsync<T>().Get();
        asset->InternalSetAssetHandle(handle);
        return asset->PerformPersistentObjectLoadAsync();
    } else {
        TMeta new_meta = {};
        new_meta.path = path;
        auto *asset = ObjectManager::CreateNewObjectAsync<T>().Get();
        new_meta.object_handle = registry.NextPersistentHandle().Get();
        if constexpr (SameAs<T, Texture2D>) {
            if (path.EndsWith(".exr") || path.EndsWith(".hdr")) {
                new_meta.format = rhi::Format::R32G32B32A32_Float;
            }
        }
        AssetDataBase::InsertMeta(new_meta);
        asset->InternalSetAssetHandle(new_meta.object_handle);
        return asset->PerformPersistentObjectLoadAsync();
    }
}

ExecFuture<ObjectHandle> AssetDataBase::Import(StringView path) {
    ProfileScope _(__func__);
    Assert(IsMainThread(), "Import只能在主线程调用");
    // 先查一下是否存在, 存在的话按现有配置重新导入
    auto query = String::Format("path = '{}'", *path);
    auto &registry = ObjectManager::GetRegistry();
    if (path.EndsWith(".fbx")) {
        return InternalImport<Mesh, MeshMeta>(query, path, registry);
    }
    if (path.EndsWith(".slang")) {
        return InternalImport<Shader, ShaderMeta>(query, path, registry);
    }
    if (path.EndsWith(".png") || path.EndsWith(".exr") || path.EndsWith(".hdr")) {
        return InternalImport<Texture2D, Texture2DMeta>(query, path, registry);
    }
    if (path.EndsWith(".ttf")) {
        return InternalImport<Font, FontMeta>(query, path, registry);
    }
    return exec::MakeExecFuture(0);
}

Object *AssetDataBase::LoadFromPath(StringView path) {
    ProfileScope _(__func__);
    const ObjectHandle obj_handle = LoadFromPathAsync(path).Get();
    return ObjectManager::GetObjectByHandle(obj_handle);
}

template<typename T, typename TMeta>
ExecFuture<ObjectHandle> InternalLoadAsync(StringView path) {
    if (const auto meta_op = AssetDataBase::QueryMeta<TMeta>(String::Format("path = '{}'", *path)); !meta_op) {
        Log(Error) << String::Format("资产{}未在资产数据库中找到.", *path);
        return exec::MakeExecFuture(0);
    } else {
        auto &meta = *meta_op;
        ObjectHandle handle = meta.object_handle;
        ObjectRegistry &registry = ObjectManager::GetRegistry();
        Object *obj = registry.GetObjectByHandle(handle);
        if (obj != nullptr) {
            return MakeExecFuture(handle);
        } else {
            T *asset = ObjectManager::CreateNewObjectAsync<T>().Get();
            asset->InternalSetAssetHandle(handle);
            return asset->PerformPersistentObjectLoadAsync();
        }
    }
}

ExecFuture<ObjectHandle> AssetDataBase::LoadFromPathAsync(StringView path) {
    ProfileScope _(__func__);
    if (path.EndsWith(".slang")) {
        return InternalLoadAsync<Shader, ShaderMeta>(path);
    }
    if (path.EndsWith(".fbx")) {
        return InternalLoadAsync<Mesh, MeshMeta>(path);
    }
    if (path.EndsWith(".png") || path.EndsWith(".exr") || path.EndsWith(".hdr")) {
        return InternalLoadAsync<Texture2D, Texture2DMeta>(path);
    }
    if (path.EndsWith(".ttf")) {
        return InternalLoadAsync<Font, FontMeta>(path);
    }
    if (path.EndsWith(".mat")) {
        // 查看资产数据库是否存在
        if (Optional<MaterialMeta> meta_op = QueryMeta<MaterialMeta>(String::Format("path = '{}'", path))) {
            const MaterialMeta &meta = *meta_op;
            ObjectHandle database_handle = meta.object_handle;
            ObjectRegistry &registry = ObjectManager::GetRegistry();
            if (const Object *obj = registry.GetObjectByHandle(database_handle); obj != nullptr) {
                return MakeExecFuture(database_handle);
            } else {
                // 创建一个instanced asset作为临时载入, 此时它被注册入registry
                Material *asset = ObjectManager::CreateNewObjectAsync<Material>().Get();
                YamlArchive archive;
                auto content = File::ReadAllText(path);
                auto old_instanced_handle = asset->GetHandle();
                if (content) {
                    // 序列化一个object 如果成功那么这个persistent handle也会被入册到registry
                    // 此时registry有两个一样的对象, 一是之前作为临时instanced object注册的, 在一个就是序列化时注册
                    if (archive.Deserialize(*content, asset, TypeOf<Material>())) {
                        auto file_handle = asset->GetHandle();
                        Assert(file_handle == database_handle, "file_handle != database_handle");
                        // 校验通过则将临时注册的取消注册
                        ObjectManager::GetRegistry().UnregisterHandle(old_instanced_handle);
                        auto rtn = asset->PerformPersistentObjectLoadAsync();
                        return rtn;
                    }
                }
                return MakeExecFuture(0);
            }
        }
    }
    return MakeExecFuture(0);
}

template<typename T>
static String QueryPath(ObjectHandle handle) {
    if constexpr (std::is_same_v<T, Shader>) {
        auto meta_op = AssetDataBase::QueryMeta<ShaderMeta>(handle);
        if (!meta_op) {
            Log(Warn) << String::Format("资产{}未在资产数据库中找到.", handle);
            return "";
        }
        auto &meta = *meta_op;
        return meta.path;
    }
    return "";
}

ExecFuture<ObjectHandle> AssetDataBase::LoadAsync(ObjectHandle handle, const Type *asset_type) {
    if (asset_type == nullptr)
        return MakeExecFuture(0);
    if (ObjectManager::IsObjectExist(handle)) {
        return MakeExecFuture(handle);
    }
    String path;
    if (asset_type == TypeOf<Shader>()) {
        path = QueryPath<Shader>(handle);
    }
    if (path.IsEmpty())
        return MakeExecFuture(0);
    return LoadFromPathAsync(path);
}

ExecFuture<ObjectHandle> AssetDataBase::LoadOrImportAsync(StringView path) {
    if (path.EndsWith(".slang")) {
        if (const auto op_meta = QueryMeta<ShaderMeta>(String::Format("path = '{}'", *path))) {
            return LoadFromPathAsync(path);
        }
        return Import(path);
    }
    if (path.EndsWith(".exr") || path.EndsWith(".png") || path.EndsWith(".hdr")) {
        if (const auto op_meta = QueryMeta<Texture2DMeta>(String::Format("path = '{}'", *path))) {
            return LoadFromPathAsync(path);
        }
        return Import(path);
    }
    if (path.EndsWith(".fbx")) {
        if (const auto op_meta = QueryMeta<MeshMeta>(String::Format("path = '{}'", *path))) {
            return LoadFromPathAsync(path);
        }
        return Import(path);
    }
    return MakeExecFuture(0);
}

Object *AssetDataBase::LoadOrImport(StringView path) { return ObjectManager::GetObjectByHandle(LoadOrImportAsync(path).Get()); }

#define CREATE_ASSET_TABLE(asset_type)                                                                                                               \
    {                                                                                                                                                \
        const Type *type = TypeOf<asset_type>();                                                                                                     \
        tables_[type] = std::move(SQLHelper::CreateTable(*db_, type));                                                                               \
    }

bool AssetDataBase::CreateAsset(Asset *asset, StringView path) {
    if (asset == nullptr) {
        Log(Error) << "传入的资产为空";
        return false;
    }
    const Type *type = asset->GetType();
    if (type == TypeOf<Mesh>() || type == TypeOf<Shader>()) {
        Log(Error) << String::Format("{}只支持导入而不支持创建.", type->GetName());
        return false;
    }
    if (type == TypeOf<Material>()) {
        if (auto exist_meta = QueryMeta<MaterialMeta>(String::Format("path = '{}'", path))) {
            Log(Error) << String::Format("{}已存在", path);
            return false;
        }
        ObjectHandle handle = ObjectManager::GetRegistry().NextPersistentHandle().Get();
        asset->InternalSetAssetHandle(handle);
        YamlArchive archive;
        String serialized_str;
        Material &material = *static_cast<Material *>(asset);
        if (!archive.Serialize(material, serialized_str)) {
            Log(Error) << "无法序列化Material.";
            return false;
        }
        File::WriteAllText(path, serialized_str);
        MaterialMeta meta;
        meta.object_handle = handle;
        meta.path = path;
        InsertMeta(meta);
        return true;
    }
    if (type == TypeOf<Texture2D>()) {
        if (auto exist_meta = QueryMeta<Texture2DMeta>(String::Format("path = '{}'", *path))) {
            Log(Error) << String::Format("{}已存在", path);
            return false;
        }
        ObjectHandle handle = ObjectManager::GetRegistry().NextPersistentHandle().Get();
        asset->InternalSetAssetHandle(handle);
        auto tex = static_cast<Texture2D *>(asset);
        Assert(StringView(tex->GetAssetPath()) == path, "创建纹理资源失败: 路径不匹配");
        Texture2DMeta meta;
        meta.object_handle = handle;
        meta.path = path;
        meta.dynamic = false;
        meta.height = tex->GetHeight();
        meta.width = tex->GetWidth();
        meta.format = tex->GetFormat();
        meta.sprites_string = tex->GetSpriteRangeString();
        InsertMeta(meta);
        return true;
    }
    return false;
}

void AssetDataBase::CreateAssetTables() {
    CREATE_ASSET_TABLE(MeshMeta);
    CREATE_ASSET_TABLE(ShaderMeta);
    CREATE_ASSET_TABLE(Texture2DMeta);
    CREATE_ASSET_TABLE(MaterialMeta);
    CREATE_ASSET_TABLE(FontMeta);
}
