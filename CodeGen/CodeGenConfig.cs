using CppAst;

namespace CodeGen;

public class CodeGenConfig
{
    public string WorkDir { get; set; } = @"C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine";

    public List<string> IncludeDirs { get; set; } =
    [
        @"Engine\Core",
        @"Engine\ThirdParty\IconFontCppHeaders",
        @"cmake-build-debug\vcpkg_installed\x64-windows\include",
        @"Generated"
    ];

    public List<string> SourceDirs { get; } =
    [
        @"Engine\Test",
        @"Engine\Core"
    ];

    public List<string> ExcludedFiles { get; } =
    [
        @"Engine\Core\Math\Math.h",
    ];

    public Dictionary<string, string> LayerDir { get; } = new()
    {
        { @"Engine\Core", "Core" },
        { @"Engine\Test", "Test" },
    };

    public List<string> Macros { get; } = ["REFLECTION"];

    public List<string> Arguments { get; } =
    [
        "-std=c++23"
    ];

    public string OutputDir { get; } = "Generated";

    public string FileCachePath { get; } = @"Generated\FileCache.json";
}