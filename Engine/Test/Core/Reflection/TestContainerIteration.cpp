/**
 * @file TestContainerIteration.cpp
 * @author Echo 
 * @Date 24-10-29
 * @brief 
 */



#include "Reflection/ITypeGetter.h"
#include "Reflection/MetaInfoManager.h"
#include "Reflection/Reflection.h"

#include "gtest/gtest.h"

class TestA : public core::ITypeGetter
{
public:
    static core::Type*              REFLECTION_Register_TestA_Registerer();
    [[nodiscard]] const core::Type* GetType() const override { return core::TypeOf<TestA>(); }

    core::Array<int32_t>            array       = {1, 2, 3};
    core::HashSet<int32_t>          hash_set    = {4, 5, 6};
    core::StaticArray<int32_t, 4>   static_aray = {7, 8, 9, 10};
    core::List<int32_t>             list        = {11, 12, 13};
    core::Set<int32_t>              set         = {14, 15, 16};
    core::Map<int32_t, int32_t>     map         = {{17, 18}, {19, 20}, {21, 22}};
    core::HashMap<int32_t, int32_t> hash_map    = {{23, 24}, {25, 26}, {27, 28}};
};

core::Type* TestA::REFLECTION_Register_TestA_Registerer()
{
    using namespace core;
    Type* type = Type::Create<TestA>("TestA");
    type->RegisterField("array", &TestA::array, offsetof(TestA, array));
    type->RegisterField("list", &TestA::list, offsetof(TestA, list));
    type->RegisterField("static_array", &TestA::static_aray, offsetof(TestA, static_aray));
    type->RegisterField("set", &TestA::set, offsetof(TestA, set));
    type->RegisterField("map", &TestA::map, offsetof(TestA, map));
    type->RegisterField("hash_map", &TestA::hash_map, offsetof(TestA, hash_map));
    return type;
}

struct TestA_MetaInfo_Register
{
    TestA_MetaInfo_Register()
    {
        core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<TestA>(), &TestA::REFLECTION_Register_TestA_Registerer);
    }
};

static TestA_MetaInfo_Register TestA_REGISTER;

TEST(Core_Reflection, TestContainerCast)
{
    TestA a;
    auto  type = core::TypeOf<TestA>();
    if (auto field = type->GetField("array"))
    {
        auto op_array = field.value()->Get<core::Array<int32_t>>(&a);
        EXPECT_EQ(op_array.has_value(), true);
        auto op_array1 = field.value()->Get<core::Array<bool>>(&a);
        EXPECT_EQ(op_array1.has_value(), false);
        auto op_array2 = field.value()->Get<int64_t>(&a);
        EXPECT_EQ(op_array2.has_value(), false);
        auto op_array3 = field.value()->Get<core::Map<int32_t, int32_t>>(&a);
        EXPECT_EQ(op_array3.has_value(), false);
        auto op_array4 = field.value()->Get<core::StaticArray<int32_t, 1>>(&a);
        EXPECT_EQ(op_array4.has_value(), false);
    }
    if (auto field1 = type->GetField("static_array"))
    {
        auto op_array = field1.value()->Get<core::Array<int32_t>>(&a);
        EXPECT_EQ(op_array.has_value(), false);
        auto op_array1 = field1.value()->Get<core::Array<bool>>(&a);
        EXPECT_EQ(op_array1.has_value(), false);
        auto op_array2 = field1.value()->Get<int64_t>(&a);
        EXPECT_EQ(op_array2.has_value(), false);
        auto op_array3 = field1.value()->Get<core::Map<int32_t, int32_t>>(&a);
        EXPECT_EQ(op_array3.has_value(), false);
        auto op_array4 = field1.value()->Get<core::StaticArray<int32_t, 4>>(&a);
        EXPECT_EQ(op_array4.has_value(), true);
    }
    if (auto field1 = type->GetField("map"))
    {
        auto op_array = field1.value()->Get<core::Array<int32_t>>(&a);
        EXPECT_EQ(op_array.has_value(), false);
        auto op_array1 = field1.value()->Get<core::Array<bool>>(&a);
        EXPECT_EQ(op_array1.has_value(), false);
        auto op_array2 = field1.value()->Get<int64_t>(&a);
        EXPECT_EQ(op_array2.has_value(), false);
        auto op_array3 = field1.value()->Get<core::Map<int32_t, int32_t>>(&a);
        EXPECT_EQ(op_array3.has_value(), true);
        auto op_array4 = field1.value()->Get<core::StaticArray<int32_t, 4>>(&a);
        EXPECT_EQ(op_array4.has_value(), false);
    }
}

TEST(Core_Reflection, TestSeqContainerIter)
{
    TestA t;
    auto  type     = core::TypeOf<TestA>();
    auto  op_field = type->GetField("set");
    EXPECT_EQ(op_field.has_value(), true);
    const auto& field = op_field.value();
    EXPECT_EQ(field->IsSequentialContainer(), true);
    auto op_set = field->Get<core::Set<int32_t>>(&t);
    EXPECT_EQ(op_set.has_value(), true);
    auto set     = op_set.value();
    auto op_view = field->CreateSequentialContainerView(&t);
    EXPECT_EQ(op_view.has_value(), true);
    auto view = op_view.value();
    int  i    = 0;
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        auto op_any = view->GetElementAt(i);
        EXPECT_EQ(op_any.HasValue(), true);
        auto op_rtn_int = core::any_cast<core::Ref<int32_t>>(op_any);
        EXPECT_EQ(op_rtn_int.has_value(), true);
        const core::Ref<int32_t>& rtn_int = op_rtn_int.value();
        EXPECT_EQ(*rtn_int, *it);
        ++i;
    }
}
