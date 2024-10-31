namespace CodeGen;
using CppAst;

class Program
{
    static void Main(string[] args)
    {
        CodeGenerator generator = new CodeGenerator();
        generator.Generate();
    }
}