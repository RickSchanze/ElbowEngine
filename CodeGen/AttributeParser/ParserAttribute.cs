namespace CodeGen.AttributeParser;

public enum ParserTarget
{
    Class,
    Struct,
    Enum
}

public class ParserAttribute(string name, ParserTarget target) : Attribute
{
    public string Name = name;
    public ParserTarget Target = target;
}