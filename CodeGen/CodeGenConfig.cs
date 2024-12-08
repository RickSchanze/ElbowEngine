using CppAst;

namespace CodeGen;

public class CodeGenConfig
{
    public string WorkDir { get; set; } = @"C:\Users\RickS\Proj\ElbowEngine";

    public List<string> IncludeDirs { get; set; } =
    [
        @"Engine",
        @"Engine\ThirdParty\IconFontCppHeaders",
        @"cmake-build-debug\vcpkg_installed\x64-windows\include",
        @"Generated",
        @"Engine\ThirdParty\tracy\public"
    ];

    public List<string> SourceDirs { get; } =
    [
        @"Engine\Test",
        @"Engine\Core",
        @"Engine\Platform",
        @"Engine\Resource",
    ];

    public List<string> ExcludedFiles { get; } =
    [
        @"Engine\Core\Math\Math.h",
    ];

    public Dictionary<string, string> LayerDir { get; } = new()
    {
        { @"Engine\Core", "Core" },
        { @"Engine\Test", "Test" },
        { @"Engine\Platform", "Platform" },
        { @"Engine\Resource", "Resource" },
    };

    public List<string> Macros { get; } = ["REFLECTION", "REFL_GEN"];

    public List<string> Arguments { get; } =
    [
        "-std=c++23"
    ];

    public string OutputDir { get; } = "Generated";

    public string FileCachePath { get; } = @"Generated\FileCache.json";
}