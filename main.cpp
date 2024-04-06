#include "Core/Log/Logger.h"
#include "Core/Object/Object.h"
#include "windows.h"

#include <iostream>

int main() {
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    Object* Refl = new TestRefl{};
    Refl->SetName(L"你好");
    std::wcout << Refl->GetName() << std::endl;
    auto t       = Refl->GetType();
    auto proerty = t.get_property("Name");
    String Str   = proerty.get_value(Refl).convert<String>();
    bool Success = proerty.set_value(Refl, String(L"不太好"));
    std::wcout << Success;
    std::wcout << Refl->GetName() << std::endl;
    Logger Logger{};
    Refl->TestYaml();
    std::wcout << Refl->GetName() << std::endl;
}
