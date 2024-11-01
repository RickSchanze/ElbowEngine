/**
 * @file TestMetaInfoGenerate.h
 * @author Echo 
 * @Date 24-10-31
 * @brief 
 */

#pragma once

#include "Core.h"

#include "TestMetaInfoGenerate.h"

#include "TestMetaInfoGenerate.generated.h"

/**
 * 测试TestMetaInfoGenerate
 */
class CLASS() TestMetaInfoGenerate : public core::ITypeGetter
{
public:
    GENERATED_BODY(TestMetaInfoGenerate);

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

namespace core
{
namespace inner
{
class CLASS(Interface) TestMetaInfoGenerate2: public core::ITypeGetter
{
    public:
    GENERATED_BODY(TestMetaInfoGenerate2);

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
}
class CLASS() TestMetaInfoGenerate1: public core::ITypeGetter
{
    public:
    GENERATED_BODY(TestMetaInfoGenerate1);

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
}
