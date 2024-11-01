using CppAst;

namespace CodeGen.AttributeParser;

public interface IClassParser
{
    string Parse(CppClass entity, KeyValuePair<string, string> attribute);
}

public interface IEnumParser
{
    string Parse(CppEnum entity, KeyValuePair<string, string> attribute);
}

public interface IStructParser
{
    string Parse(CppEnum entity, KeyValuePair<string, string> attribute);
}