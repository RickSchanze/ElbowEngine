namespace CodeGen.AttributeParser;

public enum ParserTarget
{
    Class,
    Struct,
    Enum,
    Field,
    Function,
    EnumItem
}

public class ParserAttribute(string name, ParserTarget target) : Attribute
{
    public string Name = name;
    public ParserTarget Target = target;
}