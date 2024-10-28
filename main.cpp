// #include "CoreGlobal.h"
// #include "Exception/Exception.h"
// #include "Log/Logger.h"
// #include "Path/Path.h"
#define GLFW_INCLUDE_VULKAN
// #include "EngineApplication.h"
// #include "Component/Camera.h"
#include "CoreDef.h"
#include "CoreGlobal.h"
#include "d3d12.h"
#include "Log/CoreLogCategory.h"
#include "Reflection/ITypeGetter.h"
#include "Reflection/MetaInfoManager.h"
#include "Reflection/Reflection.h"
#include "Serialization/YamlArchive.h"

class TestA : public core::ITypeGetter
{
public:
    static core::Type*              REFLECTION_Register_TestA_Registerer();
    [[nodiscard]] const core::Type* GetType() const override { return core::TypeOf<TestA>(); }

    core::Array<int32_t> array = {1, 2, 3};
    int32_t              intv;
};

core::Type* TestA::REFLECTION_Register_TestA_Registerer()
{
    using namespace core;
    Type* type = Type::Create<TestA>("TestA");
    type->RegisterField("array", &TestA::array, offsetof(TestA, array));
    type->RegisterField("intv", &TestA::intv, offsetof(TestA, intv));
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

int main()
{
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    auto a = core::TypeOf<float>();
    LOGGER.Info(LogCat::Test, "类型名称{}", a->GetName());
    auto  t = core::TypeOf<TestA>();
    TestA b{};
    b.array.push_back(30);
    auto field = t->GetField("array");
    if (field)
    {
        auto& field_value = field.value();
        if (field_value->IsSequentialContainer())
        {
            auto view_op = field_value->CreateSequentialContainerView(&b);
            if (view_op)
            {
                auto view = view_op.value();
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
                core::Any v  = view->GetElementAt(0);
                auto      vv = core::any_cast<core::Ref<int>>(v);
                if (vv.has_value())
                {
                    vv.value() = 100;
                }
                v = view->GetElementAt(1);
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
    system("pause");
    // try
    // {
    //     tool::EngineApplication App{LR"(C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine\Content)", L"肘击引擎"};
    //     App.Initialize();
    //     App.Run();
    //     App.DeInitialize();
    // }
    // catch (const Exception& e)
    // {
    //     g_logger.Exception(e);
    //     return -1;
    // }
    // catch (const std::exception& e)
    // {
    //     g_logger.StdException(e);
    //     return -1;
    // }
    // catch (...)
    // {
    //     g_logger.Error(L"捕获到了未处理的未知异常");
    //     return -1;
    // }
    // return 0;
}
