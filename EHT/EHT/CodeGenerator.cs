using System.Collections.Concurrent;
using System.Globalization;
using System.Text;
using CppAst;
using Newtonsoft.Json;

namespace EHT;

class FileReflectedEntities
{
    public Dictionary<CppClass, Dictionary<string, string>> Classes { get; set; } = new();
    public Dictionary<CppClass, Dictionary<string, string>> Structs { get; set; } = new();
    public Dictionary<CppClass, Dictionary<string, string>> Interfaces { get; set; } = new();
    public Dictionary<CppEnum, Dictionary<string, string>> Enums { get; set; } = new();
}

public static class CodeGenerator
{
    static List<string> GetModifiedFiles(List<string> files)
    {
        ConcurrentQueue<string> modifiedFiles = new();
        List<Task> tasks = new();
        Dictionary<string, string>? cache = new();
        try
        {
            cache = JsonConvert.DeserializeObject<Dictionary<string, string>>(File.ReadAllText("Cache.json"));
        }
        catch (Exception e)
        {
            cache = new Dictionary<string, string>();
        }

        foreach (var file in files)
        {
            tasks.Add(Task.Run(() =>
            {
                FileInfo fileInfo = new(file);
                if (cache.TryGetValue(file, out var value))
                {
                    if (value == fileInfo.LastWriteTime.ToString(CultureInfo.InvariantCulture))
                    {
                        return;
                    }
                }

                modifiedFiles.Enqueue(file);
            }));
        }

        Task.WaitAll(tasks.ToArray());
        return modifiedFiles.ToList();
    }

    static void UpdateCache(List<string> files)
    {
        Dictionary<string, string>? cache = new();
        try
        {
            cache = JsonConvert.DeserializeObject<Dictionary<string, string>>(File.ReadAllText("Cache.json"));
        }
        catch (Exception e)
        {
            cache = new Dictionary<string, string>();
        }

        foreach (var file in files)
        {
            if (cache.ContainsKey(file))
            {
                FileInfo info = new(file);
                cache[file] = info.LastWriteTime.ToString(CultureInfo.InvariantCulture);
            }
            else
            {
                FileInfo info = new(file);
                cache.Add(file, info.LastWriteTime.ToString(CultureInfo.InvariantCulture));
            }
        }

        File.WriteAllText("Cache.json", JsonConvert.SerializeObject(cache, Formatting.Indented));
    }

    public static bool Generate(ProjectMap projectMap, List<string> extraIncludes)
    {
        List<string> files = Directory.GetFiles(projectMap.RootDirectory, "*.hpp", SearchOption.AllDirectories).ToList();
        files = GetModifiedFiles(files);
        if (files.Count == 0) return true;
        CppParserOptions options = new CppParserOptions();
        options.IncludeFolders.AddRange(extraIncludes);
        options.IncludeFolders.Add(projectMap.RootDirectory);
        options.AdditionalArguments.Add("-std=c++23");
        options.Defines.Add("TRACY_ENABLE");
        options.Defines.Add("ENABLE_REFLECTION");
        options.ParseMacros = true;
        var compilation = CppParser.ParseFiles(files, options);
        if (compilation.HasErrors)
        {
            Console.WriteLine("[EHT] 代码生成错误:");
            foreach (var diag in compilation.Diagnostics.Messages)
            {
                if (diag.Type == CppLogMessageType.Error)
                {
                    Console.WriteLine(diag);
                }
            }
        }

        if (Generate(compilation, projectMap))
        {
            UpdateCache(files);
            return true;
        }

        return false;
    }

    static bool CheckKey(string key)
    {
        return key is "Class" or "Struct" or "Interface" or "Enum" or "Field" or "Value";
    }

    static Dictionary<string, string> ParseAttribute(string attribute, ref bool reflected)
    {
        Dictionary<string, string> attributes = new();
        var splitedAttributes = attribute.Split(",").Select(x => x.Trim()).ToList();
        foreach (string splitedAttribute in splitedAttributes)
        {
            var splittedKeyValue = splitedAttribute.Split("=").Select(x => x.Trim()).ToList();
            if (splittedKeyValue[0] == "Reflected")
            {
                reflected = true;
                continue;
            }

            attributes.Add(splittedKeyValue[0], splittedKeyValue.Count == 2 ? splittedKeyValue[1] : "");
        }

        return attributes;
    }

    static void AddClassEntity(CppClass cppClass, Dictionary<string, FileReflectedEntities> reflectedEntities)
    {
        if (cppClass.Attributes.Count == 0)
        {
            return;
        }

        bool reflected = false;
        Dictionary<string, string> attributes = ParseAttribute(cppClass.Attributes[0].Arguments, ref reflected);
        if (!reflected) return;

        if (!reflectedEntities.TryGetValue(cppClass.SourceFile, out var reflectedEntity))
        {
            reflectedEntity = new();
            reflectedEntities.Add(cppClass.SourceFile, reflectedEntity);
        }

        if (attributes.ContainsKey("Class"))
        {
            reflectedEntity.Classes.Add(cppClass, attributes);
        }
        else if (attributes.ContainsKey("Struct"))
        {
            reflectedEntity.Structs.Add(cppClass, attributes);
        }
        else if (attributes.ContainsKey("Interface"))
        {
            reflectedEntity.Interfaces.Add(cppClass, attributes);
        }
    }

    static void AddEnumEntity(CppEnum cppEnum, Dictionary<string, FileReflectedEntities> reflectedEntities)
    {
        if (cppEnum.Attributes.Count == 0)
        {
            return;
        }

        bool reflected = false;
        Dictionary<string, string> attributes = ParseAttribute(cppEnum.Attributes[0].Arguments, ref reflected);
        if (!reflected) return;

        if (!reflectedEntities.TryGetValue(cppEnum.SourceFile, out var reflectedEntity))
        {
            reflectedEntity = new();
            reflectedEntities.Add(cppEnum.SourceFile, reflectedEntity);
        }

        reflectedEntity.Enums.Add(cppEnum, attributes);
    }

    static void InternalEnumerateNamespace(CppNamespace cppNamespace, Dictionary<string, FileReflectedEntities> reflectedEntities)
    {
        foreach (var newNamespace in cppNamespace.Namespaces)
        {
            InternalEnumerateNamespace(newNamespace, reflectedEntities);
        }

        foreach (var _class in cppNamespace.Classes)
        {
            AddClassEntity(_class, reflectedEntities);
        }

        foreach (var _enum in cppNamespace.Enums)
        {
            AddEnumEntity(_enum, reflectedEntities);
        }
    }

    static Dictionary<string, FileReflectedEntities> FindAllReflectedEntity(CppCompilation compilation)
    {
        Dictionary<string, FileReflectedEntities> reflectedEntities = new();
        foreach (var _class in compilation.Classes)
        {
            AddClassEntity(_class, reflectedEntities);
        }

        foreach (var _enum in compilation.Enums)
        {
            AddEnumEntity(_enum, reflectedEntities);
        }

        foreach (var cppNamespace in compilation.Namespaces)
        {
            InternalEnumerateNamespace(cppNamespace, reflectedEntities);
        }

        return reflectedEntities;
    }

    static Dictionary<CppField, Dictionary<string, string>> FindAllFields(CppClass entity)
    {
        Dictionary<CppField, Dictionary<string, string>> result = new();
        foreach (var field in entity.Fields)
        {
            if (field.Attributes.Count == 0)
            {
                continue;
            }

            bool reflected = false;
            Dictionary<string, string> attributes = ParseAttribute(field.Attributes[0].Arguments, ref reflected);
            if (!reflected) continue;

            result.Add(field, attributes);
        }

        return result;
    }

    static Dictionary<CppEnumItem, Dictionary<string, string>> FindAllFields(CppEnum entity)
    {
        Dictionary<CppEnumItem, Dictionary<string, string>> result = new();
        foreach (var field in entity.Items)
        {
            Dictionary<string, string> attributes;
            if (field.Attributes.Count > 0)
            {
                var reflected = false;
                attributes = ParseAttribute(field.Attributes[0].Arguments, ref reflected);
            }
            else
            {
                attributes = new();
            }


            result.Add(field, attributes);
        }

        return result;
    }

    static void GenerateClassFieldCode(StringBuilder builder, Dictionary<CppField, Dictionary<string, string>> fields)
    {
        foreach (var (key, value) in fields)
        {
            builder.Append($"| refl_helper::AddField(\"{key.Name}\", &{key.FullParentName}::{key.Name})");
            foreach (var (attrKey, attrValue) in value)
            {
                if (CheckKey(attrKey)) continue;
                if (string.IsNullOrEmpty(attrValue))
                {
                    builder.Append($".Attribute(Field::FlagAttributeBits::{attrKey})");
                }
                else
                {
                    builder.Append($".Attribute(Field::ValueAttribute::{attrKey}, \"{attrValue}\")");
                }
            }

            builder.AppendLine();
        }
    }


    static bool Generate(Dictionary<string, FileReflectedEntities> fileEntities, ProjectMap map)
    {
        foreach (var (file, entities) in fileEntities)
        {
            StringBuilder header_content = new StringBuilder();
            header_content.AppendLine("// 此文件由代码生成器自动生成");
            header_content.AppendLine("#pragma once");
            foreach (var _class in entities.Classes)
            {
                header_content.AppendLine($"#define GENERATED_BODY{_class.Key.Name} \\");
                header_content.AppendLine("public: \\");
                if (_class.Key.BaseTypes.Count == 0)
                {
                    header_content.AppendLine($"typedef {_class.Key.Name} ThisClass; \\");
                }
                else
                {
                    header_content.AppendLine($"typedef ThisClass Super; \\");
                    header_content.AppendLine($"typedef {_class.Key.Name} ThisClass; \\");
                }

                header_content.AppendLine($"struct Z_ReflectionInitializer{_class.Key.Name} {{ \\");
                header_content.AppendLine($"Z_ReflectionInitializer{_class.Key.Name}() {{\\");
                header_content.AppendLine($"ReflManager::GetByRef().Register<{_class.Key.FullName}>();\\");
                header_content.AppendLine($"}}\\");
                header_content.AppendLine($"}}; \\");
                header_content.AppendLine($"static inline Z_ReflectionInitializer{_class.Key.Name} Z_ReflectionInitializer; \\");
                if (!_class.Value.ContainsKey("Abstract"))
                {
                    header_content.AppendLine($"static void ConstructSelf(void* Self) {{ new (Self) {_class.Key.Name}(); }} \\");
                    header_content.AppendLine($"static void DestructSelf(void* Self) {{ static_cast<{_class.Key.Name}*>(Self)->~{_class.Key.Name}(); }} \\");
                }

                header_content.AppendLine($"virtual const Type* GetType() const {{ return TypeOf<{_class.Key.Name}>(); }}; \\");
                header_content.AppendLine($"static const Type* GetStaticType() {{ return TypeOf<{_class.Key.Name}, true>();}}; \\");
                header_content.AppendLine($"static Type* ConstructType(); \\");
                header_content.AppendLine($"private: \\");
            }

            foreach (var _class in entities.Interfaces)
            {
                header_content.AppendLine($"#define GENERATED_BODY{_class.Key.Name} \\");
                header_content.AppendLine("public: \\");

                header_content.AppendLine($"struct Z_ReflectionInitializer{_class.Key.Name} {{ \\");
                header_content.AppendLine($"Z_ReflectionInitializer{_class.Key.Name}() {{\\");
                header_content.AppendLine($"ReflManager::GetByRef().Register<{_class.Key.FullName}>();\\");
                header_content.AppendLine($"}}\\");
                header_content.AppendLine($"}}; \\");
                header_content.AppendLine($"static inline Z_ReflectionInitializer{_class.Key.Name} Z_ReflectionInitializer; \\");
                header_content.AppendLine($"virtual const Type* GetType() const {{ return TypeOf<{_class.Key.Name}>(); }}; \\");
                header_content.AppendLine($"static const Type* GetStaticType() {{ return TypeOf<{_class.Key.Name}, true>();}}; \\");
                header_content.AppendLine($"static Type* ConstructType(); \\");
                header_content.AppendLine($"private: \\");
            }

            foreach (var _class in entities.Structs)
            {
                header_content.AppendLine($"#define GENERATED_BODY{_class.Key.Name} \\");
                header_content.AppendLine("public: \\");
                if (_class.Key.BaseTypes.Count == 0)
                {
                    header_content.AppendLine($"typedef {_class.Key.Name} ThisStruct; \\");
                }
                else
                {
                    header_content.AppendLine($"typedef ThisStruct Super; \\");
                    header_content.AppendLine($"typedef {_class.Key.Name} ThisStruct; \\");
                }

                header_content.AppendLine($"struct Z_ReflectionInitializer{_class.Key.Name} {{ \\");
                header_content.AppendLine($"Z_ReflectionInitializer{_class.Key.Name}() {{\\");
                header_content.AppendLine($"ReflManager::GetByRef().Register<{_class.Key.FullName}>();\\");
                header_content.AppendLine($"}}\\");
                header_content.AppendLine($"}}; \\");
                header_content.AppendLine($"static inline Z_ReflectionInitializer{_class.Key.Name} Z_ReflectionInitializer; \\");
                if (!_class.Value.ContainsKey("Abstract"))
                {
                    Console.WriteLine($"[EUT] {file}: struct {_class.Key.Name}不能有Abstract属性");
                    return false;
                }

                header_content.AppendLine($"virtual const Type* GetType() const {{ return TypeOf<{_class.Key.Name}>(); }}; \\");
                header_content.AppendLine($"static const Type* GetStaticType() {{ return TypeOf<{_class.Key.Name}, true>();}}; \\");
                header_content.AppendLine($"static Type* ConstructType(); \\");
            }

            foreach (var _enum in entities.Enums)
            {
                header_content.AppendLine();
                header_content.AppendLine($"Type* ConstructType_{_enum.Key.Name}();");
                header_content.AppendLine($"void Z_CallRegisterFunction{_enum.Key.Name}();");
                header_content.AppendLine($"struct Z_ReflectionInitializer{_enum.Key.Name} {{ ");
                header_content.AppendLine($"Z_ReflectionInitializer{_enum.Key.Name}() {{");
                header_content.AppendLine($"Z_CallRegisterFunction{_enum.Key.Name}();");
                header_content.AppendLine($"}}");
                header_content.AppendLine($"}}; ");
                header_content.AppendLine($"extern Z_ReflectionInitializer{_enum.Key.Name} Z_ValueReflectionInitializer{_enum.Key.Name};");
            }

            if (map.TryGetFileProjectInfo(file, out var info))
            {
                string outHeader = Path.Combine(info.IncludeDirectory, Path.GetFileName(file).Replace(".hpp", ".generated.hpp"));
                File.WriteAllText(outHeader, header_content.ToString());
            }
            else
            {
                Console.WriteLine($"[EHT] 没有为{file}找到对应的工程文件夹.");
                return false;
            }

            StringBuilder source_content = new();
            source_content.AppendLine("// 此文件由工具自动生成");
            source_content.AppendLine($"#include \"{file.Replace("\\", "/")}\"");
            foreach (var _class in entities.Classes)
            {
                source_content.AppendLine();
                source_content.AppendLine($"Type* {_class.Key.FullName}::ConstructType() {{");
                source_content.AppendLine($"return Type::Create<{_class.Key.FullName}>(\"{_class.Key.FullName}\")");

                if (_class.Key.BaseTypes.Count > 0)
                {
                    string parents = "";
                    bool firstParent = true;
                    for (int i = 0; i < _class.Key.BaseTypes.Count; i++)
                    {
                        if (_class.Key.BaseTypes[i].Type is CppClass parentClass)
                        {
                            if (parentClass.Attributes.Count > 0)
                            {
                                if (parentClass.Attributes[0].Arguments.Contains("Reflected"))
                                {
                                    if (firstParent)
                                    {
                                        parents += _class.Key.BaseTypes[i].Type.FullName;
                                    }
                                    else
                                    {
                                        parents += ", " + _class.Key.BaseTypes[i].Type.FullName;
                                    }
                                }
                            }
                        }
                    }

                    source_content.Append($"| refl_helper::AddParents<{parents}>()");
                }

                foreach (var (key, value) in _class.Value)
                {
                    if (key == "Abstract") continue;
                    if (CheckKey(key)) continue;
                    if (string.IsNullOrEmpty(value))
                    {
                        source_content.Append($"| refl_helper::Attribute(Type::FlagAttributeBits::{key})");
                    }
                    else
                    {
                        string v = value.Trim('"').Trim().Trim('"');
                        source_content.Append($"| refl_helper::Attribute(Type::ValueAttribute::{key}, \"{v}\")");
                    }
                }

                source_content.AppendLine();

                GenerateClassFieldCode(source_content, FindAllFields(_class.Key));

                source_content.AppendLine(";}");
            }

            foreach (var _class in entities.Interfaces)
            {
                source_content.AppendLine();
                source_content.AppendLine($"Type* {_class.Key.FullName}::ConstructType() {{");
                source_content.AppendLine($"return Type::Create<{_class.Key.FullName}>(\"{_class.Key.FullName}\")");

                if (_class.Key.BaseTypes.Count > 0)
                {
                    string parents = "";
                    for (int i = 0; i < _class.Key.BaseTypes.Count; i++)
                    {
                        if (_class.Key.BaseTypes[i].Type is CppClass parentClass)
                        {
                            if (parentClass.Attributes.Count > 0)
                            {
                                if (parentClass.Attributes[0].Arguments.Contains("Reflected"))
                                {
                                    parents += _class.Key.BaseTypes[i].Type.FullName;
                                    if (i != _class.Key.BaseTypes.Count - 1)
                                    {
                                        parents += ", ";
                                    }
                                }
                            }
                        }
                    }

                    source_content.Append($"| refl_helper::AddParents<{parents}>()");
                }

                foreach (var (key, value) in _class.Value)
                {
                    if (key == "Abstract") continue;
                    if (CheckKey(key)) continue;
                    if (string.IsNullOrEmpty(value))
                    {
                        source_content.Append($"| refl_helper::Attribute(Type::FlagAttributeBits::{key})");
                    }
                    else
                    {
                        string v = value.Trim('"').Trim().Trim('"');
                        source_content.Append($"| refl_helper::Attribute(Type::ValueAttribute::{key}, \"{v}\")");
                    }
                }

                // 不会为Interface生成字段反射
                source_content.AppendLine(";}");
            }

            foreach (var _class in entities.Structs)
            {
                source_content.AppendLine();
                source_content.AppendLine($"Type* {_class.Key.FullName}::ConstructType() {{");
                source_content.AppendLine($"return Type::Create<{_class.Key.FullName}>(\"{_class.Key.FullName}\")");

                if (_class.Key.BaseTypes.Count > 0)
                {
                    string parents = "";
                    for (int i = 0; i < _class.Key.BaseTypes.Count; i++)
                    {
                        if (_class.Key.BaseTypes[i].Type is CppClass parentClass)
                        {
                            if (parentClass.Attributes.Count > 0)
                            {
                                if (parentClass.Attributes[0].Arguments.Contains("Reflected"))
                                {
                                    parents += _class.Key.BaseTypes[i].Type.FullName;
                                    if (i != _class.Key.BaseTypes.Count - 1)
                                    {
                                        parents += ", ";
                                    }
                                }
                            }
                        }
                    }

                    source_content.Append($"| refl_helper::AddParents<{parents}>()");
                }

                foreach (var (key, value) in _class.Value)
                {
                    if (key == "Abstract") continue;
                    if (CheckKey(key)) continue;
                    if (string.IsNullOrEmpty(value))
                    {
                        source_content.Append($"| refl_helper::Attribute(Type::FlagAttributeBits::{key})");
                    }
                    else
                    {
                        string v = value.Trim('"').Trim().Trim('"');
                        source_content.Append($"| refl_helper::Attribute(Type::ValueAttribute::{key}, \"{v}\")");
                    }
                }

                source_content.AppendLine();

                GenerateClassFieldCode(source_content, FindAllFields(_class.Key));

                source_content.AppendLine(";}");
            }

            foreach (var _enum in entities.Enums)
            {
                source_content.AppendLine($"void Z_CallRegisterFunction{_enum.Key.Name}() {{");
                source_content.AppendLine($"ReflManager::GetByRef().Register<{_enum.Key.FullName}>(&ConstructType_{_enum.Key.Name});");
                source_content.AppendLine($"}}");
                source_content.AppendLine($"Z_ReflectionInitializer{_enum.Key.Name} Z_ValueReflectionInitializer{_enum.Key.Name}{{}};");
                source_content.AppendLine($"Type* ConstructType_{_enum.Key.Name}() {{");
                source_content.AppendLine($"return Type::Create<{_enum.Key.FullName}>(\"{_enum.Key.FullName}\")");
                foreach (var (key, value) in _enum.Value)
                {
                    if (CheckKey(key)) continue;
                    if (string.IsNullOrEmpty(value))
                    {
                        source_content.Append($"| refl_helper::Attribute(Type::FlagAttributeBits::{key})");
                    }
                    else
                    {
                        string v = value.Trim('"').Trim().Trim('"');
                        source_content.Append($"| refl_helper::Attribute(Type::ValueAttribute::{key}, \"{v}\")");
                    }
                }

                var enumFields = FindAllFields(_enum.Key);
                foreach (var (key, value) in enumFields)
                {
                    source_content.Append($"| refl_helper::AddEnumField(\"{key.Name}\", std::to_underlying({_enum.Key.FullName}::{key.Name}))");
                    foreach (var (attrKey, attrValue) in value)
                    {
                        if (CheckKey(attrKey)) continue;
                        if (string.IsNullOrEmpty(attrKey))
                        {
                            source_content.Append($".Attribute(Field::FlagAttributeBits::{attrKey})");
                        }
                        else
                        {
                            string v = attrValue.Trim('"').Trim().Trim('"');
                            source_content.Append($".Attribute(Field::ValueAttribute::{attrKey}, \"{v}\")");
                        }
                    }

                    source_content.AppendLine("");
                }

                source_content.AppendLine(";}");
            }


            string outSource = Path.Combine(info.SourceDirectory, Path.GetFileName(file).Replace(".hpp", ".generated.cpp"));
            File.WriteAllText(outSource, source_content.ToString());
        }

        return true;
    }

    static bool Generate(CppCompilation compilation, ProjectMap map)
    {
        var reflectedEntities = FindAllReflectedEntity(compilation);
        return Generate(reflectedEntities, map);
    }
}