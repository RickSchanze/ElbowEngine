﻿// See https://aka.ms/new-console-template for more information


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
        output = "C:/Users/kita/Documents/Projects/ElbowEngine/cmake-build-debug/Engine";
    }

    if (string.IsNullOrEmpty(input))
    {
        input = "C:/Users/kita/Documents/Projects/ElbowEngine/Engine";
    }

    if (string.IsNullOrEmpty(include))
    {
        include = @"C:\Users\kita\Documents\Projects\ElbowEngine\cmake-build-debug\vcpkg_installed\x64-windows\include";
    }

    Console.WriteLine($"[EHT] 输出路径={output}");
    Console.WriteLine($"[EHT] 输入路径={input}");

    var cMakeProjects = CMakeProjectProcessor.FindCMakeProjects(input);
    var projectMap = CMakeProjectProcessor.CreateFolders(output, cMakeProjects);
    projectMap.RootDirectory = input;
    if (!CodeGenerator.Generate(projectMap, [include]))
    {
        throw new Exception("生成失败");
    }
}, outputOption, inputOption, includeOption);

await rootCommand.InvokeAsync(args);