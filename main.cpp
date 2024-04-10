#include "Core/CoreGlobal.h"
#include "Core/Exception/Exception.h"
#include "Core/Log/Logger.h"
#include "Core/Serialization/Interfaces.h"
#include "Core/Serialization/YamlSerializer.h"

#include "Core/Serialization/TestSerialization.h"
#include "windows.h"


int main() {
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    try {
        auto O1 = New<TestSerialization>(L"测试序列化功能对象");
        // YamlSerializer s{};
        // const String Name = L"wdc.yaml";
        // s.SerializeFile(O1, Name);
        const AnsiString Name = "wdc.yaml";
        YAML::Node N = YAML::LoadFile(Name);
        for (auto CN:N) {
            // auto str = CN.second.as<std::string>();
        }
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
