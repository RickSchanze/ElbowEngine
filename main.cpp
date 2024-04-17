#include "CoreGlobal.h"
#include "Engine/Function/Path/Path.h"
#include "Exception/Exception.h"
#include "Log/Logger.h"
#include "RHI/Vulkan/Core.h"
#include "windows.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


int main() {
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    try {
        glfwInit();
        uint32 Count;
        const char** Extensions = glfwGetRequiredInstanceExtensions(&Count);
        Vulkan::Application App{VK_MAKE_VERSION(1, 1, 1), VK_MAKE_VERSION(1, 1, 1)};
        App.Init(Extensions, Count);
        App.Finalize();
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
