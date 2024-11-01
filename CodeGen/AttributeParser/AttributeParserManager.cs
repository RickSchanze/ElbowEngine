using System.Reflection;

namespace CodeGen.AttributeParser;

public static class AttributeParserManager
{
    public static readonly Dictionary<string, IClassParser> ClassParsers = new();
    public static readonly Dictionary<string, IEnumParser> EnumParsers = new();
    public static readonly Dictionary<string, IStructParser> StructParsers = new();

    public static IClassParser? GetClassParser(string name)
    {
        return ClassParsers.GetValueOrDefault(name);
    }

    public static IEnumParser? GetEnumParser(string name)
    {
        return EnumParsers.GetValueOrDefault(name);
    }

    public static IStructParser? GetStructParser(string name)
    {
        return StructParsers.GetValueOrDefault(name);
    }

    static AttributeParserManager()
    {
        // 获取当前程序集中所有带有 ParserAttribute 标记的类
        var parserTypes = from type in Assembly.GetExecutingAssembly().GetTypes()
            let attribute = (ParserAttribute)type.GetCustomAttributes(typeof(ParserAttribute), false).FirstOrDefault()
            where attribute != null
            select new { Type = type, Attribute = attribute };
        foreach (var parserType in parserTypes)
        {
            switch (parserType.Attribute.Target)
            {
                case ParserTarget.Class:
                    if (parserType.Type.IsClass && !parserType.Type.IsAbstract)
                    {
                        if (typeof(IClassParser).IsAssignableFrom(parserType.Type))
                        {
                            var instance = Activator.CreateInstance(parserType.Type);
                            ClassParsers.Add(parserType.Attribute.Name, (instance as IClassParser)!);
                        }
                        else
                        {
                            Console.WriteLine($"Class parser {parserType.Type.Name} does not implement IClassParser");
                        }
                    }

                    break;

                case ParserTarget.Enum:
                    if (parserType.Type.IsValueType && !parserType.Type.IsEnum)
                    {
                        if (typeof(IEnumParser).IsAssignableFrom(parserType.Type))
                        {
                            var instance = Activator.CreateInstance(parserType.Type);
                            EnumParsers.Add(parserType.Attribute.Name, (instance as IEnumParser)!);
                        }
                        else
                        {
                            Console.WriteLine($"Class parser {parserType.Type.Name} does not implement IEnumParser");
                        }
                    }

                    break;

                case ParserTarget.Struct:
                    if (typeof(IStructParser).IsAssignableFrom(parserType.Type))
                    {
                        var instance = Activator.CreateInstance(parserType.Type);
                        StructParsers.Add(parserType.Attribute.Name, (instance as IStructParser)!);
                    }
                    else
                    {
                        Console.WriteLine($"Class parser {parserType.Type.Name} does not implement IStructParser");
                    }

                    break;
            }
        }
    }
}