/**
 * @file TestMetaInfoGenerate.cpp
 * @author Echo 
 * @Date 24-11-5
 * @brief 
 */
#include "TestMetaInfoGenerate.h"

#include "TestMetaInfoGenerate.generated.h"

#include <gtest/gtest.h>

GENERATED_SOURCE()

TEST(Core_MetaInfoGenerate, Enum)
{
    auto t = core::TypeOf<core::TestEnum>();
    EXPECT_EQ(t->GetName(), "core::TestEnum");
}
