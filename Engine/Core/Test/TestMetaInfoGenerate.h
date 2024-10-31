/**
 * @file TestMetaInfoGenerate.h
 * @author Echo 
 * @Date 24-10-31
 * @brief 
 */

#pragma once

#include "Core.h"

/**
 * 测试TestMetaInfoGenerate
 */
class CLASS(Test = true, Ref = 12, OOO) TestMetaInfoGenerate : public core::ITypeGetter
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
