#include "Core/CoreGlobal.h"
#include "Core/Exception/Exception.h"
#include "windows.h"
#include "Core/Log/Logger.h"

int a = 50;

int main() {
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    try {
        const auto* O1 = New<Object>();
        delete O1;
        auto O2 = New<Object, ENewReturnType::SharedPtr>();
        auto O3 = New<Object, ENewReturnType::UniquePtr>();
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
