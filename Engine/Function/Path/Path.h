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
private:
    // 只能用来初始化工作路径
    Path() = default;

public:
    /**
     * 初始化一个路径
     * @param PathStr
     */
    explicit Path(StringView PathStr);

    Path(const Path& OtherPath) = default;

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
    [[nodiscard]] bool IsExist() const;

    /**
     * 将Path转换为绝对路径字符串
     * @return
     */
    [[nodiscard]] String ToString() const;

    /**
     * 以此Path为路径创建一个目录
     */
    void CreateDirectory() const;

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
    static Optional<Path> GetWorkPath() noexcept;

    /**
     * 获取项目元数据文件路径
     * @return
     */
    static Optional<Path> GetProjectMetaFilePath() noexcept;

    /**
     * 创建当前路径指示的文件
     */
    void CreateFile() const;

private:
    // 项目工作路径
    static inline Path* sProjectWorkPath = nullptr;

    std::filesystem::path mPath;
};
