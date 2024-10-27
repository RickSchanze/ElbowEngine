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
#include "Reflection/MetaInfoManager.h"
#include "Reflection/Reflection.h"
#include "Serialization/YamlArchive.h"

int main()
{
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    auto a = core::TypeOf<bool>();
    LOGGER.Info(LogCat::Test, "类型名称{}", a.GetName());
    // LOGGER.Info(LogCat::Test, "测试一下");
    // core::StringView v = "你好";
    // LOGGER.Warn(LogCat::Test, "测试一下Str {}", v);
    // LOGGER.Error(LogCat::Test, "测试一下Str {}", v);
    // core::YamlArchive ar;
    // auto*             obj = New<core::Object>();
    // ar.BeginSerialize();
    // ar << *obj;
    // ar.EndSerialize();
    // std::cout << ar.ToString();
    // system("pause");
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
