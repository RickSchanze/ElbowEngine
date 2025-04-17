// See https://aka.ms/new-console-template for more information


using System.CommandLine;
using EHT;

var rootCommand = new RootCommand("示例工具");

// 定义选项
var outputOption = new Option<string>("--output", "输出路径");
var inputOption = new Option<string>("--input", "输入扫描路径");
var includeOption = new Option<string>("--include", "输入包含路径");

rootCommand.AddOption(outputOption);
rootCommand.AddOption(inputOption);
rootCommand.AddOption(includeOption);

// 设置处理逻辑
rootCommand.SetHandler((output, input, include) =>
{
    if (string.IsNullOrEmpty(output))
    {
        output = "C:/Users/Echo/Documents/Projects/ElbowEngine/cmake-build-debug/Engine";
    }

    if (string.IsNullOrEmpty(input))
    {
        input = "C:/Users/Echo/Documents/Projects/ElbowEngine/Engine";
    }

    if (string.IsNullOrEmpty(include))
    {
        include = @"C:\Users\Echo\Documents\Projects\ElbowEngine\cmake-build-debug\vcpkg_installed\x64-windows\include";
    }

    var cMakeProjects = CMakeProjectProcessor.FindCMakeProjects(input);
    var projectMap = CMakeProjectProcessor.CreateFolders(output, cMakeProjects);
    projectMap.RootDirectory = input;
    if (!CodeGenerator.Generate(projectMap, [include]))
    {
        throw new Exception("生成失败");
    }
}, outputOption, inputOption, includeOption);

await rootCommand.InvokeAsync(args);