using CppAst;

namespace CodeGen;

public class FileCppInfo
{
    public List<CppClass> Classes { get; } = new();
    public List<CppEnum> Enums { get; } = new();
    public string FilePath { get; }

    private void ParseNamespace(CppNamespace ns)
    {
        foreach (var cppClass in ns.Classes)
        {
            if (cppClass.SourceFile == FilePath)
            {
                Classes.Add(cppClass);
            }
        }

        foreach (var cppEnum in ns.Enums)
        {
            if (cppEnum.SourceFile == FilePath)
            {
                Enums.Add(cppEnum);
            }
        }

        foreach (var nss in ns.Namespaces)
        {
            ParseNamespace(nss);
        }
    }

    public FileCppInfo(CppCompilation compilation, string filePath)
    {
        FilePath = filePath;
        foreach (var cppClass in compilation.Classes)
        {
            if (cppClass.SourceFile == filePath)
            {
                Classes.Add(cppClass);
            }
        }

        foreach (var cppEnum in compilation.Enums)
        {
            if (cppEnum.SourceFile == filePath)
            {
                Enums.Add(cppEnum);
            }
        }

        foreach (var ns in compilation.Namespaces)
        {
            ParseNamespace(ns);
        }
    }
}