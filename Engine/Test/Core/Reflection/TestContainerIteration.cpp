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
    type->RegisterField("static_aray", &TestA::static_aray, offsetof(TestA, static_aray));
    type->RegisterField("map", &TestA::map, offsetof(TestA, map));
    type->RegisterField("hash_map", &TestA::hash_map, offsetof(TestA, hash_map));
    return type;
}

struct TestA_MetaInfo_Register
{
    TestA_MetaInfo_Register()
    {
        core::MetaDataRegisterer registerer;
        registerer.name       = "TestA";
        registerer.registerer = &TestA::REFLECTION_Register_TestA_Registerer;
        core::MetaInfoManager::Get()->RegisterTypeRegisterer(typeid(TestA).hash_code(), registerer);
    }
};

static TestA_MetaInfo_Register TestA_REGISTER;

void TestSequenceIteration(core::StringView name)
{
    auto  t = core::TypeOf<TestA>();
    TestA b{};
    b.array.push_back(30);
    auto field = t->GetField(name);
    if (field)
    {
        auto& field_value = field.value();
        if (field_value->IsSequentialContainer())
        {
            auto view_op = field_value->CreateSequentialContainerView(&b);
            if (view_op)
            {
                auto view = view_op.value();
                view->ForEach([](int i, const core::Any& item) {
                    const auto v = core::any_cast<core::Ref<int>>(item);
                    EXPECT_EQ(v.has_value(), true);
                    // EXPECT_EQ(v.value(), b.s)
                });
                core::Any v  = view->GetElementAt(0);
                auto      vv = core::any_cast<core::Ref<int>>(v);
                if (vv.has_value())
                {
                    vv.value() = 100;
                }
                v        = view->GetElementAt(1);
                auto vv2 = core::any_cast<core::Ref<int>>(v);
                if (vv2.has_value())
                {
                    vv2.value() = 200;
                }
                view->ForEach([](const core::Any& item) {
                    const auto v = core::any_cast<core::Ref<int>>(item);
                    if (!v.has_value())
                    {
                        LOGGER.Error(LogCat::Test, "Error: {}", GetEnumString(v.error()));
                    }
                    else
                    {
                        LOGGER.Info(LogCat::Test, "Value: {}", v.value());
                    }
                });
            }
        }
    }
}

void ContainerCast()
{
    TestA a;
    auto  type  = core::TypeOf<TestA>();
    auto  field = type->GetField("array");
    if (field)
    {
        auto op_array = field.value()->Get<core::Array<int32_t>>(&a);
        EXPECT_EQ(op_array.has_value(), true);
        auto op_array1 = field.value()->Get<core::Array<bool>>(&a);
        EXPECT_EQ(op_array1.has_value(), false);
        auto op_array2 = field.value()->Get<int64_t>(&a);
        EXPECT_EQ(op_array2.has_value(), false);
    }
}

TEST(Core_Reflection, TestContainerCast)
{
    TestA a;
    auto  type  = core::TypeOf<TestA>();
    auto  field = type->GetField("array");
    if (field)
    {
        auto op_array = field.value()->Get<core::Array<int32_t>>(&a);
        EXPECT_EQ(op_array.has_value(), true);
        auto op_array1 = field.value()->Get<core::Array<bool>>(&a);
        EXPECT_EQ(op_array1.has_value(), false);
        auto op_array2 = field.value()->Get<int64_t>(&a);
        EXPECT_EQ(op_array2.has_value(), false);
    }
}
