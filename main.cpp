#define GLFW_INCLUDE_VULKAN
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/CoreDef.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/YamlArchive.h"
#include "d3d12.h"
#include "Platform/FileSystem/Path.h"

#include <fstream>

int main()
{
    // 让std::wcout 顺利运行
    setlocale(LC_ALL, "zh_CN");
    // 让spdlog不产生乱码
    SetConsoleOutputCP(65001);
    LOGGER.Info(logcat::Test, "TestInfo");
    if (!platform::Path::SetProjectPath("C:/Users/Echo/SyncWork/Work/Projects/ElbowEngine/Content"))
    {
        LOGGER.Critical(logcat::Core, "Set project path failed, abort program.");
        return -1;
    }
    auto cfg = core::GetConfig<core::CoreConfig>();
}
