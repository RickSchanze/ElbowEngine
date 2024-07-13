#include "CoreGlobal.h"
#include "Path/Path.h"
#include "Exception/Exception.h"
#include "Log/Logger.h"
#define GLFW_INCLUDE_VULKAN
#include "EngineApplication.h"


int main() {
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    try {
        Tool::EngineApplication App{LR"(C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine\Content)", L"肘击引擎"};
        App.Initialize();
        App.Run();
        App.Finitialize();
    } catch (const Exception& e) {
        gLogger.Exception(e);
        return -1;
    } catch (const std::exception& e) {
        gLogger.StdException(e);
        return -1;
    } catch (...) {
        gLogger.Error(L"捕获到了未处理的未知异常");
        return -1;
    }
    return 0;
}
