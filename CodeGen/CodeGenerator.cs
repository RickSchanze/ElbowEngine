using System.Collections.Concurrent;
using System.Security.Cryptography;
using System.Text;
using CppAst;
using Newtonsoft.Json;

namespace CodeGen;

public class FileCppInfo
{
    public HashSet<CppClass> Classes { get; } = new();
    public HashSet<CppEnum> Enums { get; } = new();
    public string FilePath { get; init; }
}

public class CodeGeneratorHelper
{
    static void ProcessClass(Dictionary<string, FileCppInfo> cache, CppClass cppClass, List<string> processingFiles)
    {
        string? sourceFile = cppClass.SourceFile?.Replace("\\", "/");
        if (sourceFile == null) return;
        if (!processingFiles.Contains(sourceFile)) return;
        if (!cppClass.Attributes.Any(x => x.Arguments.Contains("Reflection"))) return;
        if (cache.TryGetValue(sourceFile, out var cppInfo))
        {
            cppInfo.Classes.Add(cppClass);
        }
        else
        {
            FileCppInfo newCppInfo = new()
            {
                FilePath = sourceFile,
            };
            newCppInfo.Classes.Add(cppClass);
            cache.Add(sourceFile, newCppInfo);
        }
    }

    static void ProcessEnum(Dictionary<string, FileCppInfo> cache, CppEnum cppEnum, List<string> processingFiles)
    {
        string? sourceFile = cppEnum.SourceFile?.Replace("\\", "/");
        if (sourceFile == null) return;
        if (!processingFiles.Contains(sourceFile)) return;
        if (!cppEnum.Attributes.Any(x => x.Arguments.Contains("Reflection"))) return;
        if (cache.TryGetValue(sourceFile, out var cppInfo))
        {
            cppInfo.Enums.Add(cppEnum);
        }
        else
        {
            FileCppInfo newEnumInfo = new()
            {
                FilePath = sourceFile,
            };
            newEnumInfo.Enums.Add(cppEnum);
            cache.Add(sourceFile, newEnumInfo);
        }
    }

    static void ProcessNameSpace(Dictionary<string, FileCppInfo> cache, CppNamespace cppNamespace,
        List<string> processingFiles)
    {
        foreach (var cppClass in cppNamespace.Classes)
        {
            ProcessClass(cache, cppClass, processingFiles);
        }

        foreach (var cppEnum in cppNamespace.Enums)
        {
            ProcessEnum(cache, cppEnum, processingFiles);
        }

        foreach (var ns in cppNamespace.Namespaces)
        {
            ProcessNameSpace(cache, ns, processingFiles);
        }
    }

    public static Dictionary<string, FileCppInfo> FindAllReflectedClasses(CppCompilation compilation,
        List<string> processingFiles)
    {
        Dictionary<string, FileCppInfo> cache = new();
        foreach (var cppClass in compilation.Classes)
        {
            ProcessClass(cache, cppClass, processingFiles);
        }

        foreach (var cppEnum in compilation.Enums)
        {
            ProcessEnum(cache, cppEnum, processingFiles);
        }

        foreach (var cppNamespace in compilation.Namespaces)
        {
            ProcessNameSpace(cache, cppNamespace, processingFiles);
        }

        return cache;
    }
}

public class CodeGenerator
{
    private readonly CodeGenConfig _config = new();

    private List<string> AbsoluteIncludeDirs =>
        _config.IncludeDirs.Select(dir => Path.Combine(_config.WorkDir, dir)).ToList();

    private List<string> AbsoluteSourceDirs =>
        _config.SourceDirs.Select(dir => Path.Combine(_config.WorkDir, dir)).ToList();

    private List<string> ExcludedFiles =>
        _config.ExcludedFiles.Select(file => Path.Combine(_config.WorkDir, file)).ToList();

    private void EnsureOutputExist()
    {
        var outputAbsPath = Path.Combine(_config.WorkDir, _config.OutputDir);
        if (!Directory.Exists(outputAbsPath))
        {
            Directory.CreateDirectory(outputAbsPath);
        }
    }

    private bool HasError;

    private void GenerateFile(FileCppInfo cppInfo, string outputPath)
    {
        string layer = "";
        foreach (var layerName in _config.LayerDir)
        {
            if (cppInfo.FilePath.Contains(layerName.Key))
            {
                layer = layerName.Value;
                break;
            }
        }

        string fileName = Path.GetFileNameWithoutExtension(cppInfo.FilePath);
        string generatedCopyFileName = string.IsNullOrEmpty(layer)
            ? $"{fileName}.generated.copy.h"
            : $"{layer}.{fileName}.generated.copy.h";
        string generatedFileName =
            string.IsNullOrEmpty(layer) ? $"{fileName}.generated.h" : $"{layer}.{fileName}.generated.h";
        generatedCopyFileName = Path.Combine(outputPath, generatedCopyFileName);
        generatedFileName = Path.Combine(outputPath, generatedFileName);
        using (var fs = new FileStream(generatedCopyFileName, FileMode.Create))
        {
            using var sw = new StreamWriter(fs, Encoding.UTF8);
            // 生成文件头
            GenerateHeader(sw, cppInfo);
            sw.WriteLine();

            // 生成类信息
            GenerateClasses(sw, cppInfo);
        }

        File.Copy(generatedCopyFileName, generatedFileName, true);
        File.Delete(generatedCopyFileName);
    }

    private void GenerateFiles(List<string> paths)
    {
        var options = new CppParserOptions();
        options.IncludeFolders.AddRange(AbsoluteIncludeDirs);
        options.Defines.AddRange(_config.Macros);
        options.AdditionalArguments.AddRange(_config.Arguments);
        paths = paths.Where(p => !ExcludedFiles.Contains(p)).ToList();
        var compilation = CppParser.ParseFiles(paths, options);
        if (compilation.HasErrors)
        {
            foreach (var diagnostic in compilation.Diagnostics.Messages)
            {
                Console.WriteLine(diagnostic);
            }

            HasError = true;

            return;
        }

        for (int i = 0; i < paths.Count; i++)
        {
            var path = paths[i];
            paths[i] = path.Replace("\\", "/");
        }

        Dictionary<string, FileCppInfo> all = CodeGeneratorHelper.FindAllReflectedClasses(compilation, paths);

        var outputAbsPath = Path.Combine(_config.WorkDir, _config.OutputDir);
        List<Task> tasks = new();
        foreach (var value in all.Values)
        {
            tasks.Add(
                Task.Run(() => { GenerateFile(value, outputAbsPath); })
            );
        }

        Task.WaitAll(tasks.ToArray());
    }

    private void GenerateEnums(StreamWriter sw, FileCppInfo info)
    {
        var validEnums =
            info.Enums.Where(x => x.Attributes.Any(attribute => attribute.Arguments.Trim() == "Reflection"));
        foreach (var validEnum in validEnums)
        {
            sw.WriteLine("{ \\");
            GenerateEnum(sw, validEnum);
            sw.WriteLine("} \\");
        }
    }

    private void GenerateEnum(StreamWriter sw, CppEnum validEnum)
    {
        var enumName = validEnum.FullName;
        enumName = enumName.Replace("::", ".");
        var attributes = ParseAttribute(validEnum.Attributes);
        if (attributes.ContainsKey("Name"))
        {
            enumName = attributes["Name"];
            attributes.Remove("Name");
            enumName = enumName.Trim('"').Trim();
        }

        sw.Write($"Type* type = Type::Create<{validEnum.FullName}>(\"{enumName}\")");
        var comment = validEnum.Comment?.ToString();
        if (!string.IsNullOrEmpty(comment))
        {
            comment = comment.Replace("\r\n", "\\n");
            comment = comment.Replace("\n", "\\n");
            sw.Write($"->SetComment(\"{comment}\")");
        }

        var attribues = ParseAttribute(validEnum.Attributes);
        foreach (var attribute in attributes)
        {
            if (attribute.Key == "Flag")
            {
                sw.Write($"->SetAttribute(Type::FlagAttribute::{attribute.Key})");
                continue;
            }

            throw new Exception($"Unknown attribute: {attribute.Key}");
        }

        sw.WriteLine("; \\");
        sw.WriteLine($"using enum {validEnum.FullName}; \\");
        GenerateEnumFields(sw, validEnum.Items);
        sw.WriteLine(
            $"core::MetaInfoManager::Get()->RegisterType(core::RTTITypeInfo::Create<{validEnum.FullName}>(), type); \\");
    }

    private void GenerateEnumFields(StreamWriter sw, CppContainerList<CppEnumItem> validEnumItems)
    {
        foreach (var validEnumItem in validEnumItems)
        {
            var attributes = ParseAttribute(validEnumItem.Attributes);
            var itemName = validEnumItem.Name;
            if (attributes.ContainsKey("Name"))
            {
                itemName = attributes["Name"];
                attributes.Remove("Name");
                itemName = itemName.Trim('"').Trim();
            }

            sw.Write($"type->Internal_RegisterEnumValue({validEnumItem.Name} ,\"{itemName}\")");
            var comment = validEnumItem.Comment?.ToString();
            if (!string.IsNullOrEmpty(comment))
            {
                comment = comment.Replace("\n", "\\n");
                sw.Write($"->SetComment(\"{comment}\")");
            }

            foreach (var attribute in attributes)
            {
                if (_config.EnumFlagAttrs.Contains(attribute.Key))
                {
                    sw.Write($"->SetAttribute(FieldInfo::FlagAttribute::{attribute.Key})");
                    continue;
                }

                if (_config.EnumValueAttrs.Contains(attribute.Key))
                {
                    sw.Write($"->SetAttribute(FieldInfo::ValueAttribute::{attribute.Key}, \"{attribute.Value}\")");
                    continue;
                }

                throw new Exception("Unknown attribute: " + attribute.Key);
            }

            sw.WriteLine("; \\");
        }
    }

    Dictionary<string, string> ParseAttribute(List<CppAttribute> attributes)
    {
        Dictionary<string, string> result = new();
        foreach (var attribute in attributes)
        {
            var attr = attribute.Arguments;
            int idx = attr.IndexOf("=", StringComparison.Ordinal);

            string key = attr.Trim().Trim('"').Trim();
            string value = "-";
            if (idx != -1)
            {
                key = attr.Substring(0, idx);
                key = key.Trim().Trim('"').Trim();
                value = attr[(idx + 1)..];
                value = value.Trim().Trim('"').Trim();
            }

            result.Add(key, value);
        }

        result.Remove("Reflection");
        return result;
    }

    private void GenerateClasses(StreamWriter sw, FileCppInfo info)
    {
        sw.WriteLine("#undef GENERATED_IMPLEMENTATION");
        sw.WriteLine("#define GENERATED_IMPLEMENTATION \\");
        var classes =
            info.Classes.Where(x => x.Attributes.Any(attribute => attribute.Arguments.Trim() == "Reflection"));
        foreach (var @class in classes)
        {
            sw.WriteLine($"core::Type* {@class.FullName}::REFLECTION_Register_{@class.Name}_Registerer() {{ \\");
            GenerateClass(sw, @class);
            sw.WriteLine("} \\");
        }

        sw.WriteLine();
    }

    private void GenerateClass(StreamWriter sw, CppClass class_)
    {
        var attributes = ParseAttribute(class_.Attributes);
        var className = class_.FullName;
        className = className.Replace("::", ".");
        if (attributes.ContainsKey("Name"))
        {
            className = attributes["Name"];
            attributes.Remove("Name");
            className = className.Trim('"').Trim();
        }

        var comment = class_.Comment?.ToString();

        sw.WriteLine("using namespace core; \\");
        sw.Write($"Type* type = Type::Create<{class_.FullName}>(\"{className}\")");
        foreach (var baseType in class_.BaseTypes)
        {
            if (baseType.Type is not CppClass cls) continue;
            if (cls.Attributes.Any((attribute => attribute.ToString().Contains("Reflection"))))
            {
                sw.Write($"->Internal_AddParent(TypeOf<{baseType.Type.FullName}>())");
            }
        }

        if (!string.IsNullOrEmpty(comment))
        {
            comment = comment.Replace(System.Environment.NewLine, "\\n");
            sw.Write($"->SetComment(\"{comment}\")");
        }

        foreach (var attr in attributes)
        {
            if (_config.TypeFlagAttrs.Contains(attr.Key))
            {
                sw.Write($"->SetAttribute(Type::{attr.Key})");
                continue;
            }

            if (_config.TypeValueAttrs.Contains(attr.Key))
            {
                var attrValue = attr.Value;
                sw.Write($"->SetAttribute(core::Type::ValueAttribute::{attr.Key}, \"{attrValue}\")");
                continue;
            }

            throw new Exception($"Error: Unknown attribute {attr.Key} = {attr.Value}");
        }

        sw.WriteLine("; \\");
        GenerateClassFields(sw, class_);
        sw.WriteLine("return type; \\");
    }

    private void GenerateClassFields(StreamWriter sw, CppClass class_)
    {
        class_.Fields.Where(x => x.Attributes.Any(attribute => attribute.Arguments.Trim() == "Reflection"))
            .ToList()
            .ForEach(field => GenerateClassField(sw, field));
    }

    void GenerateClassField(StreamWriter sw, CppField field)
    {
        if (field.Parent is not CppClass cppClass) return;
        var classFullName = cppClass.FullName;
        var attributes = ParseAttribute(field.Attributes);
        var fieldName = field.Name;
        if (attributes.ContainsKey("Name"))
        {
            fieldName = attributes["Name"];
            attributes.Remove("Name");
            fieldName = fieldName.Trim('"').Trim();
        }

        sw.Write(
            $"type->Internal_RegisterField(\"{fieldName}\", &{classFullName}::{field.Name}, offsetof({classFullName}, {field.Name}))");
        var comment = field.Comment?.ToString();
        if (!string.IsNullOrEmpty(comment))
        {
            comment = comment.Replace("\n", "\\n");
            sw.Write($"->SetComment(\"{comment}\")");
        }


        foreach (var attr in attributes)
        {
            if (_config.FieldFlagAttrs.Contains(attr.Key))
            {
                sw.Write($"->SetAttribute(FieldInfo::{attr.Key})");
                continue;
            }

            if (_config.FieldValueAttrs.Contains(attr.Key))
            {
                var attrValue = string.IsNullOrEmpty(attr.Value) ? "Config" : attr.Value;
                sw.Write($"->SetAttribute(core::FieldInfo::ValueAttribute::{attr.Key}, \"{attrValue}\")");
                continue;
            }

            throw new Exception($"Error: Unknown attribute {attr.Key} = {attr.Value}");
        }


        sw.WriteLine("; \\");
    }

    private void GenerateHeader(StreamWriter sw, FileCppInfo info)
    {
        sw.WriteLine("// 此文件由工具自动生成，请勿手动修改");
        sw.WriteLine(@"#undef GENERATED_ALL_CLASS_BODY");
        sw.WriteLine(@"#define GENERATED_ALL_CLASS_BODY \");
        sw.WriteLine("using namespace core; \\");
        var reflectedClasses = info.Classes.Where(x =>
        {
            return x.Attributes.Any(attribute => attribute.Arguments.Trim() == "Reflection");
        });
        List<string> processedName = new();
        var cppClasses = reflectedClasses as CppClass[] ?? reflectedClasses.ToArray();
        foreach (var class_ in cppClasses)
        {
            string classFullName = class_.FullName;
            string className = class_.Name;
            if (processedName.Contains(className))
            {
                throw new Exception($"{info.FilePath}: Duplicate class name: {className}");
            }

            processedName.Add(className);
            sw.WriteLine(
                $"core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<{classFullName}>(), &{classFullName}::REFLECTION_Register_{className}_Registerer); \\");
            GenerateSourceBodyForClass(sw, class_);
        }

        GenerateEnums(sw, info);
        sw.WriteLine();

        foreach (var cppClass in cppClasses)
        {
            GenerateHeaderBodyForClass(sw, cppClass);
        }
    }

    private void GenerateSourceBodyForClass(StreamWriter sw, CppClass cppClass)
    {
        var attributes = ParseAttribute(cppClass.Attributes);
        if (!attributes.ContainsKey("Interface"))
        {
            sw.WriteLine(
                $"core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<{cppClass.FullName}>(), &{cppClass.FullName}::ConstructAt, &{cppClass.FullName}::DestroyAt); \\");
        }
    }

    private void GenerateHeaderBodyForClass(StreamWriter sw, CppClass cppClass)
    {
        sw.WriteLine($"#undef GENERATED_BODY_IMPL_{cppClass.Name}");
        sw.WriteLine($"#define GENERATED_BODY_IMPL_{cppClass.Name} \\");
        var arttribues = ParseAttribute(cppClass.Attributes);
        if (!arttribues.ContainsKey("Interface"))
        {
            sw.WriteLine(
                $"static void ConstructAt(void* ptr) {{ new (ptr) {cppClass.FullName}(); }} \\");
            sw.WriteLine(
                $"static void DestroyAt(void* ptr) {{ static_cast<{cppClass.FullName}*>(ptr)->~{cppClass.Name}(); }} \\");
        }

        sw.WriteLine();
    }

    private static string ComputSHA256(string filePath)
    {
        using FileStream stream = File.OpenRead(filePath);
        using SHA256 sha = SHA256.Create();
        byte[] hash = sha.ComputeHash(stream);
        return BitConverter.ToString(hash).Replace("-", "").ToLower();
    }

    public void Generate()
    {
        EnsureOutputExist();
        string fileCache = Path.Combine(_config.WorkDir, _config.FileCachePath);
        ConcurrentDictionary<string, string> cache;
        try
        {
            cache = JsonConvert.DeserializeObject<ConcurrentDictionary<string, string>>(File.ReadAllText(fileCache)) ??
                    new();
        }
        catch
        {
            cache = new();
        }

        // 获取文件夹及其子文件夹中所有 .h 文件
        List<Task> tasks = new();
        List<string> files = new();
        object lockObj = new();
        foreach (var sourceDir in AbsoluteSourceDirs)
        {
            var hFiles = Directory.GetFiles(sourceDir, "*.h", SearchOption.AllDirectories);
            foreach (var hFile in hFiles)
            {
                tasks.Add(Task.Run(() =>
                {
                    var dirty = true;
                    var newHash = ComputSHA256(hFile);
                    if (cache.TryGetValue(hFile, out var oldHash))
                    {
                        if (oldHash == newHash)
                        {
                            dirty = false;
                        }
                    }

                    cache[hFile] = newHash;
                    if (!dirty) return;
                    lock (lockObj)
                    {
                        files.Add(hFile);
                    }
                }));
            }
        }

        Task.WaitAll(tasks.ToArray());
        GenerateFiles(files);

        if (!HasError)
        {
            Task.WaitAll(tasks.ToArray());
            string newCache = JsonConvert.SerializeObject(cache, Formatting.Indented);
            File.WriteAllText(fileCache, newCache);
        }
    }
}