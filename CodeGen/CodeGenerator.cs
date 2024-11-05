using System.Text;
using CodeGen.AttributeParser;
using CppAst;

namespace CodeGen;

public class CodeGenerator
{
    private readonly CodeGenConfig _config = new();

    private List<string> AbsoluteIncludeDirs =>
        _config.IncludeDirs.Select(dir => Path.Combine(_config.WorkDir, dir)).ToList();

    private List<string> AbsoluteSourceDirs =>
        _config.SourceDirs.Select(dir => Path.Combine(_config.WorkDir, dir)).ToList();

    private void EnsureOutputExist()
    {
        var outputAbsPath = Path.Combine(_config.WorkDir, _config.OutputDir);
        if (!Directory.Exists(outputAbsPath))
        {
            Directory.CreateDirectory(outputAbsPath);
        }
    }

    private void GenerateFile(string path)
    {
        var oldFileName = Path.GetFileName(path);
        if (!oldFileName.EndsWith(".h"))
        {
            throw new Exception("Only support .h file");
        }

        var options = new CppParserOptions();
        options.IncludeFolders.AddRange(AbsoluteIncludeDirs);
        options.Defines.AddRange(_config.Macros);
        options.AdditionalArguments.AddRange(_config.Arguments);
        var compilation = CppParser.ParseFile(path, options);
        if (compilation.HasErrors)
        {
            foreach (var diagnostic in compilation.Diagnostics.Messages)
            {
                Console.WriteLine(diagnostic);
            }
        }

        var outputAbsPath = Path.Combine(_config.WorkDir, _config.OutputDir);

        oldFileName = oldFileName[..oldFileName.IndexOf(".h", StringComparison.Ordinal)];
        var outputCopyFilePath = Path.Combine(outputAbsPath, Path.GetFileName(oldFileName + ".copy.generated.h"));
        var outputFilePath = Path.Combine(outputAbsPath, Path.GetFileName(oldFileName + ".generated.h"));
        using (var fs = new FileStream(outputCopyFilePath, FileMode.Create))
        {
            using var sw = new StreamWriter(fs, Encoding.UTF8);
            var info = new FileCppInfo(compilation, path);

            // 生成文件头
            GenerateHeader(sw, info);
            sw.WriteLine();

            // 生成类信息
            GenerateClasses(sw, info);
        }

        File.Copy(outputCopyFilePath, outputFilePath, true);
        File.Delete(outputCopyFilePath);
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
            sw.Write($"->SetComment(\"{comment}\")");
        }

        foreach (var attr in attributes)
        {
            IEnumParser? parser = AttributeParserManager.GetEnumParser(attr.Key);
            if (parser == null)
            {
                throw new Exception($"No parser found for attribute: {attr.Key}");
            }

            sw.Write(parser.Parse(validEnum, attr));
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
                sw.Write($"->SetComment(\"{comment}\")");
            }

            foreach (var attr in attributes)
            {
                IEnumItemParser? parser = AttributeParserManager.GetEnumItemParser(attr.Key);
                if (parser == null)
                {
                    throw new Exception($"No parser found for attribute: {attr.Key}");
                }

                sw.Write(parser.Parse(validEnumItem, attr));
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
            if (attr?.Contains('=') ?? false)
            {
                var parts = attr.Split('=');
                result[parts[0].Trim()] = parts[1].Trim();
            }
            else
            {
                if (attr == null) continue;
                attr = attr.Trim();
                result[attr] = "";
            }
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
        if (attributes.ContainsKey("Name"))
        {
            className = attributes["Name"];
            attributes.Remove("Name");
            className = className.Trim('"').Trim();
        }

        var comment = class_.Comment?.ToString();

        sw.WriteLine("using namespace core; \\");
        sw.Write($"Type* type = Type::Create<{class_.FullName}>(\"{className}\")");
        if (!string.IsNullOrEmpty(comment))
        {
            sw.Write($"->SetComment(\"{comment}\")");
        }

        foreach (var attr in attributes)
        {
            IClassParser? parser = AttributeParserManager.GetClassParser(attr.Key);
            if (parser == null)
            {
                throw new Exception($"No parser found for attribute: {attr.Key}");
            }

            sw.Write(parser.Parse(class_, attr));
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
            sw.Write($"->SetComment(\"{comment}\")");
        }

        foreach (var attr in attributes)
        {
            IFieldParser? parser = AttributeParserManager.GetFieldParser(attr.Key);
            if (parser == null)
            {
                throw new Exception($"No parser found for attribute: {attr.Key}");
            }

            sw.Write(parser.Parse(field, attr));
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
        foreach (var class_ in reflectedClasses)
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
        }

        // 枚举
        GenerateEnums(sw, info);
    }

    public void Generate()
    {
        EnsureOutputExist();
        // 获取文件夹及其子文件夹中所有 .h 文件
        Task.WaitAll((from sourceDir in AbsoluteSourceDirs
            from file in Directory.GetFiles(sourceDir, "*.h", SearchOption.AllDirectories)
            select Task.Run(() => { GenerateFile(file); })).ToArray());
    }
}