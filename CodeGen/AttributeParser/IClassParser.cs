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

public interface IFieldParser
{
    string Parse(CppField entity, KeyValuePair<string, string> attribute);
}

public interface IFunctionParser
{
    string Parse(CppFunction entity, KeyValuePair<string, string> attribute);
}

public interface IEnumItemParser
{
    string Parse(CppEnumItem entity, KeyValuePair<string, string> attribute);
}