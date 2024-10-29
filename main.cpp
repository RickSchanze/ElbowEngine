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

int main()
{
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    LOGGER.Info(LogCat::Test, "this");
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
