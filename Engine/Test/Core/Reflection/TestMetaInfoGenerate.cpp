/**
 * @file TestMetaInfoGenerate.cpp
 * @author Echo 
 * @Date 24-11-5
 * @brief 
 */
#include "TestMetaInfoGenerate.h"



#include <gtest/gtest.h>

#include "Test.TestMetaInfoGenerate.generated.h"

GENERATED_SOURCE()

TEST(Core_MetaInfoGenerate, Enum)
{
    auto t = core::TypeOf<core::TestEnum>();
    EXPECT_EQ(t->GetFullName(), "core::TestEnum");
}
