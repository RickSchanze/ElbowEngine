/**
 * @file TestMetaInfoGenerate.h
 * @author Echo 
 * @Date 24-11-5
 * @brief 
 */

#pragma once

/**
 * @file TestMetaInfoGenerate.h
 * @author Echo
 * @Date 24-10-31
 * @brief
 */

#pragma once

#include "Core/Core.h"

#include "Core/Reflection/ITypeGetter.h"
#include GEN_HEADER("Test.TestMetaInfoGenerate.generated.h")

/**
 * 测试TestMetaInfoGenerate
 */
class CLASS() TestMetaInfoGenerate : public core::ITypeGetter
{
public:
    GENERATED_CLASS(TestMetaInfoGenerate);

private:
    /// 测试属性注释
    PROPERTY()
    int32_t value1_ = 0;

    PROPERTY()
    bool value2_ = true;

    PROPERTY()
    core::Array<int32_t> value3_ = {1, 2, 3};

    PROPERTY()
    core::HashMap<int32_t, int32_t> value4_ = {{1, 2}, {3, 4}};
};

namespace core
{
namespace inner
{
class CLASS(Interface, Name = "NewName") TestMetaInfoGenerate2 : public core::ITypeGetter
{
public:
    GENERATED_CLASS(TestMetaInfoGenerate2);

private:
    PROPERTY()
    int32_t value1_ = 0;

    PROPERTY()
    bool value2_ = true;

    PROPERTY()
    core::Array<int32_t> value3_ = {1, 2, 3};

    PROPERTY()
    core::HashMap<int32_t, int32_t> value4_ = {{1, 2}, {3, 4}};
};
}   // namespace inner
class CLASS() TestMetaInfoGenerate1 : public core::ITypeGetter
{
public:
    GENERATED_CLASS(TestMetaInfoGenerate1);

private:
    PROPERTY()
    int32_t value1_ = 0;

    PROPERTY()
    bool value2_ = true;

    PROPERTY()
    core::Array<int32_t> value3_ = {1, 2, 3};

    PROPERTY()
    core::HashMap<int32_t, int32_t> value4_ = {{1, 2}, {3, 4}};
};


enum class ENUM() TestEnum
{
    A,
    B,
    C,
    D
};


}

