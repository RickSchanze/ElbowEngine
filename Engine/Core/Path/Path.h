/**
 * @file Path.h
 * @author Echo 
 * @Date 24-4-15
 * @brief 
 */

#pragma once
#include <filesystem>

#include "CoreDef.h"


class Path {

public:
    Path() = default;
    /**
     * 初始化一个路径
     * @param PathStr
     */
    Path(StringView PathStr);

    Path(const wchar_t* PathStr) : Path(StringView(PathStr)) {}

    Path(const Path& OtherPath) = default;

    static Path FromStdPath(const std::filesystem::path& InPath) {
        Path NewPath;
        NewPath.path_ = InPath;
        return NewPath;
    }

    /**
     * 设置工作路径
     * @param PathStr
     */
    static void SetProjectWorkPath(StringView PathStr);

    /**
     * 此Path是否有效
     * 当含有非法字符时无效
     * 路径不存在时不算无效
     * @return
     */
    bool IsExist() const;

    /**
     * 将Path转换为绝对路径字符串
     * @return
     */
    String ToAbsoluteString() const;

    String ToRelativeString() const;


    const char* ToRelativeCStr();

    /**
     * 以此Path为路径创建一个目录
     */
    void CreateDir() const;

    /**
     * 判断这个path指示的目录是不是空
     * 如果不是目录返回false 是目录不为空返回false 如果此路径不存在返回false
     * @return
     */
    [[nodiscard]] bool IsFolderEmpty() const;

    /**
     * 判断这个path指示的是不是目录
     * A. C:/Test -> true
     * B. C:/Test/ -> true
     * C. C:/Test/P -> true
     * C. C:/Test/P.jpg -> false
     * @return
     */
    [[nodiscard]] bool IsFolder(bool bMustExist = false) const;

    /**
     * 获取工作目录 未设置则返回空
     * @return
     */
    static TOptional<Path> GetWorkPath() noexcept;

    /**
     * 获取项目元数据文件路径
     * @return
     */
    static TOptional<Path> GetProjectMetaFilePath() noexcept;

    /**
     * 创建当前路径指示的文件
     */
    void CreateFileA() const;

    Path GetParentPath() const;

    bool operator==(const Path& Other) const { return path_ == Other.path_; }

    Path operator/(const Path& Other) const {
        Path NewPath;
        NewPath.path_ = path_ / Other.path_;
        return NewPath;
    }

    bool EndsWith(const String& subfix)const;

    String GetFileName() const;

    std::string ToAbsoluteAnsiString() const;

    std::string ToRelativeAnsiString() const;

    AnsiString ReadAllText() const;

    /**
     * 以二进制Char读取文件
     * @param output 如果读取失败，那么output会被清零
     */
    void ReadAllBinary(TArray<char>& output) const;
    /**
     * Shader编译将会使用这个
     * @param output
     */
    void ReadAllBinary(TArray<uint32_t>& output) const;

    /**
     * 写入二进制 不存在自动创建
     * @param binary
     */
    void WriteAllBinary(const TArray<char>&binary) const;
    /**
     * shader编译将会使用这个
     * @param binary
     */
    void WriteAllBinary(const TArray<uint32_t>& binary) const;

    /**
     * 写入文本 不存在自动创建
     * @param text
     */
    void WriteAllText(const AnsiString& text)const;

private:
    std::filesystem::path GetStdFullPath() const;

    // 项目工作路径
    static inline Path* s_project_work_path_ = nullptr;

    std::filesystem::path path_;

    AnsiString ansi_string_cache_;
};

// Path的哈希
template<>
struct std::hash<Path> {
    size_t operator()(const Path& InPath) const noexcept {
        return std::hash<String>()(InPath.ToAbsoluteString());
    }
};
