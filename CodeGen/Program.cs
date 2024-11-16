using System.Diagnostics;

namespace CodeGen;
using CppAst;

class Program
{
    static void Main(string[] args)
    {
        Stopwatch sw = new Stopwatch();
        sw.Start();
        CodeGenerator generator = new CodeGenerator();
        generator.Generate();
        sw.Stop();
        Console.WriteLine($"Elapsed: {sw.ElapsedMilliseconds} ms");
    }
}