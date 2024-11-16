/**
 * @file TestArchiveSerialization.cpp
 * @author Echo 
 * @Date 24-11-7
 * @brief 
 */

#include "TestArchiveSerialization.h"
#include "Core/Core.h"
#include "Serialization/YamlArchive.h"
#include <gtest/gtest.h>

#include "Test.TestArchiveSerialization.generated.h"

#include <fstream>

GENERATED_SOURCE()

TEST(Core_Archive_Serialization, TestArchiveSerialization)
{
    core::UniquePtr<core::Archive> ar = New<core::YamlArchive>();
    ar->BeginSerialize();
    TestArchiveSerialization test;
    core::Any                a = test;
    *ar << a;
    ar->EndSerialize();
    std::ofstream out("test.yaml");
    out << ar->ToString();
}
