﻿using CppAst;

namespace CodeGen;

public class CodeGenConfig
{
    public string WorkDir { get; set; } = @"C:\Users\Echo\Documents\Projects\ElbowEngine";

    public List<string> IncludeDirs { get; set; } =
    [
        @"Engine",
        @"Engine\ThirdParty\IconFontCppHeaders",
        @"cmake-build-debug\vcpkg_installed\x64-windows\include",
        @"Generated",
        @"Engine\ThirdParty\tracy\public",
        @"C:\VulkanSDK\1.3.296.0\Include"
    ];

    public List<string> SourceDirs { get; } =
    [
        @"Engine\Core",
        @"Engine\Platform",
        @"Engine\Resource",
        @"Engine\Func",
    ];

    public List<string> ExcludedFiles { get; } =
    [
        @"Engine\Core\Math\Math.h",
        @"Engine\Core\Base\Ranges.h",
    ];

    public Dictionary<string, string> LayerDir { get; } = new()
    {
        { @"Engine/Core", "Core" },
        { @"Engine/Test", "Test" },
        { @"Engine/Platform", "Platform" },
        { @"Engine/Resource", "Resource" },
        { @"Engine/Func", "Func" },
    };

    public List<string> Macros { get; } = ["REFLECTION", "REFL_GEN"];

    public List<string> Arguments { get; } =
    [
        "-std=c++23"
    ];

    public string OutputDir { get; } = "Generated";

    public string FileCachePath { get; } = @"Generated\FileCache.json";

    public readonly List<string> TypeValueAttrs = ["Config", "Category", "SQLTable"];
    public readonly List<string> TypeFlagAttrs = ["Interface", "Trivial"];
    
    public readonly List<string> EnumValueAttrs = ["Label"];
    public readonly List<string> EnumFlagAttrs = [];
    
    public readonly List<string> FieldValueAttrs = ["Getter", "Setter", "Label", "Category", "EnableWhen", "EnumFlag", "SQLAttr"];
    public readonly List<string> FieldFlagAttrs = ["Transient", "Hidden"];
}