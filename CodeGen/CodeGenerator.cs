using CppAst;

namespace CodeGen;

public class CodeGenerator
{
    private readonly CodeGenConfig _config = new();

    private List<string> AbsoluteIncludeDirs =>
        _config.IncludeDirs.Select(dir => Path.Combine(_config.WorkDir, dir)).ToList();

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
        else
        {
            var outputAbsPath = Path.Combine(_config.WorkDir, _config.OutputDir);
            var outputFilePath = Path.Combine(outputAbsPath, Path.GetFileName(path));
            File.WriteAllText(outputFilePath, compilation.ToString());
        }
    }

    public void Generate()
    {
        EnsureOutputExist();
        GenerateFile(
            "C:\\Users\\Echo\\SyncWork\\Work\\Projects\\ElbowEngine\\Engine\\Core\\Test\\TestMetaInfoGenerate.h");
    }
}