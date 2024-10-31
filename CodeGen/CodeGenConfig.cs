using CppAst;

namespace CodeGen;

public class CodeGenConfig
{
    public string WorkDir { get; set; } = @"C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine";

    public List<string> IncludeDirs { get; set; } =
    [
        @"Engine\Core",
        @"Engine\ThirdParty\IconFontCppHeaders",
        @"cmake-build-debug\vcpkg_installed\x64-windows\include"
    ];

    public List<string> Macros { get; set; } = ["REFLECTION"];

    public List<string> Arguments { get; set; } =
    [
        "-std=c++23"
    ];
    
    public string OutputDir { get; set; } = "Generated";
}