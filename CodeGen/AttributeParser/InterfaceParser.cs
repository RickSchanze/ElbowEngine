using CppAst;

namespace CodeGen.AttributeParser;

[Parser("Interface", ParserTarget.Class)]
public class InterfaceParser : IClassParser
{
    public string Parse(CppClass entity, KeyValuePair<string, string> attribute)
    {
        return "->SetAttribute(Type::Interface)";
    }
}