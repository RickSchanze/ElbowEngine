import typing
from abc import abstractmethod
from typing import List, override
from pathlib import Path
from clang.cindex import Config, Cursor, CursorKind
from Config import ElbowEngineCodeGenConfig
from enum import Enum

import logging

Config.set_library_file(ElbowEngineCodeGenConfig.lib_clang_path)

logging.basicConfig(level=ElbowEngineCodeGenConfig.debug_level, format='[%(levelname)s] [%(asctime)s] %(message)s', datefmt='%Y-%d-%m %H:%M:%S')
logger = logging.getLogger()


def get_full_qualified_name(node: Cursor) -> str:
    if node.lexical_parent is not None:
        return get_full_qualified_name(node.lexical_parent) + "::" + node.spelling
    return ""


def get_base_class(node: Cursor):
    child: Cursor
    bases = []
    for child in node.get_children():
        if child.kind == CursorKind.CXX_BASE_SPECIFIER:
            bases.append(child.spelling)
    return bases


def parse_attr_str(attr_str: str) -> typing.Dict[str, str]:
    if attr_str.strip('\"') == '':
        return {}
    attr_map: typing.Dict[str, str] = {}
    attr_array = [attr.strip(" ") for attr in attr_str.split(",")]
    for attr in attr_array:
        attr_split = attr.split("=")
        if len(attr_split) != 2:
            attr_map[attr_split[0].strip("\"").strip().strip("\"")] = ""
        else:
            attr_map[attr_split[0].strip("\"").strip().strip("\"")] = attr_split[1].strip("\"").strip().strip("\"")
    if "Property" in attr_map:
        attr_map.pop("Property")
    if "Function" in attr_map:
        attr_map.pop("Function")
    if "Value" in attr_map:
        attr_map.pop("Value")
    # 为了保证不依赖anno str的生成器运行
    attr_map["__fixed_item__"] = "__fixed_item__"
    return attr_map


class ReflTraitNodeType(Enum):
    STRUCT = 0
    CLASS = 1
    ENUM = 2


class ReflTraitNode:
    def __init__(self, node: Cursor, anno_str: str, node_type: ReflTraitNodeType):
        self.refl_func_node: Cursor | None = node
        self.refl_anno_str: str | None = anno_str
        self.refl_node_type: ReflTraitNodeType = node_type


class ReflFunction:
    def __init__(self, node: Cursor, anno_str: str):
        self.refl_property_node: Cursor | None = node
        if self.refl_property_node is None:
            raise Exception("node must not be None")
        if self.refl_property_node.kind != CursorKind.CXX_METHOD:
            raise Exception("node must be function declaration")
        self.attr_str: str | None = anno_str
        self.__parse()

    def __parse(self):
        self.name = self.refl_property_node.spelling
        self.attr_map = parse_attr_str(self.attr_str)


class ReflProperty:
    def __init__(self, node: Cursor, anno_str: str):
        self.refl_property_node: Cursor | None = node
        if self.refl_property_node is None:
            raise Exception("node must not be None")
        if self.refl_property_node.kind != CursorKind.FIELD_DECL:
            raise Exception("node must be field declaration")
        self.attr_str: str | None = anno_str
        self.__parse()

    def __parse(self):
        self.name = self.refl_property_node.spelling
        self.attr_map = parse_attr_str(self.attr_str)
        self.type_name = self.refl_property_node.type.spelling


class ReflClass:
    def __init__(self, node: Cursor, anno_node: ReflTraitNode):
        self.properties: List[ReflProperty] = []
        self.functions: List[ReflFunction] = []
        self.refl_class_node: Cursor | None = node

        if self.refl_class_node is None:
            raise Exception("node must not be None")
        if self.refl_class_node.kind != CursorKind.CLASS_DECL:
            raise Exception("node must be class declaration")
        if anno_node is None:
            raise Exception("anno_node must not be None")
        if anno_node.refl_node_type != ReflTraitNodeType.CLASS:
            raise Exception("anno_node must be class declaration")
        self.trait_node = anno_node
        self.__parse()

    def __parse(self):
        self.name: str = self.refl_class_node.spelling
        self.full_name: str = get_full_qualified_name(self.refl_class_node).lstrip("::")
        self.attr_str: str = eval(self.trait_node.refl_anno_str)
        self.bases = get_base_class(self.refl_class_node)
        self.attr_map: typing.Dict[str, str] = parse_attr_str(self.attr_str)

    def append_property(self, prop: ReflProperty):
        if prop is None:
            raise Exception("property must not be None")
        if prop not in self.properties:
            self.properties.append(prop)

    def append_function(self, func: ReflFunction):
        if func is None:
            raise Exception("function must not be None")
        if func not in self.functions:
            self.functions.append(func)


class ReflStruct:
    def __init__(self, node: Cursor, anno_node: ReflTraitNode):
        self.properties: List[ReflProperty] = []
        self.refl_struct_node: Cursor | None = node

        if self.refl_struct_node is None:
            raise Exception("node must not be None")
        if self.refl_struct_node.kind != CursorKind.STRUCT_DECL:
            raise Exception("node must be struct declaration")
        if anno_node is None:
            raise Exception("anno_node must not be None")
        if anno_node.refl_node_type != ReflTraitNodeType.STRUCT:
            raise Exception("anno_node must be class declaration")
        self.trait_node = anno_node
        self.__parse()

    def __parse(self):
        self.name: str = self.refl_struct_node.spelling
        self.full_name: str = get_full_qualified_name(self.refl_struct_node).lstrip("::")
        self.attr_str: str = self.trait_node.refl_anno_str
        self.bases = get_base_class(self.refl_struct_node)
        self.attr_map = parse_attr_str(self.attr_str)

    def append_property(self, prop: ReflProperty):
        if prop is None:
            raise Exception("property must not be None")
        if prop not in self.properties:
            self.properties.append(prop)


class ReflEnumConstant:
    def __init__(self, node: Cursor, attr_str: str = ""):
        if node is None:
            raise Exception("node must not be None")
        self.node = node
        self.name = node.spelling
        self.attr_str = attr_str
        self.attr_map = parse_attr_str(self.attr_str)


class ReflEnum:
    def __init__(self, node: Cursor, anno_node: ReflTraitNode):
        self.constants: List[ReflEnumConstant] = []
        self.refl_enum_node: Cursor | None = node

        if self.refl_enum_node is None:
            raise Exception("node must not be None")
        if self.refl_enum_node.kind != CursorKind.ENUM_DECL:
            raise Exception("node must be enum declaration")
        if anno_node is None:
            raise Exception("anno_node must not be None")
        if anno_node.refl_node_type != ReflTraitNodeType.ENUM:
            raise Exception("anno_node must be enum declaration")
        self.trait_node = anno_node
        self.__parse()

    def __parse(self):
        self.name: str = self.refl_enum_node.spelling
        self.full_name: str = get_full_qualified_name(self.refl_enum_node).lstrip("::")
        self.attr_str: str = self.trait_node.refl_anno_str
        self.attr_map = parse_attr_str(self.attr_str)

    def append_constant(self, prop: ReflEnumConstant):
        if prop is None:
            raise Exception("property must not be None")
        if prop not in self.constants:
            self.constants.append(prop)


class ASTParseResult:
    def __init__(self):
        self.classes: List[ReflClass] = []
        self.structs: List[ReflStruct] = []
        self.enums: List[ReflEnum] = []

    def append_class(self, class_: ReflClass):
        if class_ is None:
            raise Exception("class must not be None")
        if class_ not in self.classes:
            self.classes.append(class_)

    def append_struct(self, struct_: ReflStruct):
        if struct_ is None:
            raise Exception("struct must not be None")
        if struct_ not in self.structs:
            self.structs.append(struct_)

    def append_enum(self, enum: ReflEnum):
        if enum is None:
            raise Exception("enum must not be None")
        if enum not in self.enums:
            self.enums.append(enum)


class ASTParser:
    def __init__(self):
        self.current_trait_node: ReflTraitNode | None = None
        self.current_refl_node: ReflClass | ReflStruct | None = None
        self.current_refl_node_end_level: int = 0
        self.result: ASTParseResult = ASTParseResult()
        self.has_error: bool = False

    def get_result(self) -> ASTParseResult | None:
        if self.has_error:
            return None
        return self.result

    def __find_attr_node(self, node: Cursor) -> Cursor | None:
        for child in node.get_children():
            if child.kind == CursorKind.ANNOTATE_ATTR:
                return child
        return None

    def ast_travel(self, node: Cursor, level: int = 0):
        if self.has_error:
            return
        kind = node.kind
        spelling = str(node.spelling)

        # 如果是函数声明，看它反射的类型
        if kind == CursorKind.FUNCTION_DECL:
            if spelling.startswith('REFLECTION_CLASS_TRAIT'):
                param_decl: Cursor = next(node.get_children())
                unexposed_expr: Cursor = next(param_decl.get_children())
                string_literal: Cursor = next(unexposed_expr.get_children())
                self.current_trait_node = ReflTraitNode(node, string_literal.spelling, ReflTraitNodeType.CLASS)
                return
            if spelling.startswith('REFLECTION_STRUCT_TRAIT'):
                param_decl: Cursor = next(node.get_children())
                unexposed_expr: Cursor = next(param_decl.get_children())
                string_literal: Cursor = next(unexposed_expr.get_children())
                self.current_trait_node = ReflTraitNode(node, string_literal.spelling, ReflTraitNodeType.STRUCT)
                return
            if spelling.startswith('REFLECTION_ENUM_TRAIT'):
                param_decl: Cursor = next(node.get_children())
                unexposed_expr: Cursor = next(param_decl.get_children())
                string_literal: Cursor = next(unexposed_expr.get_children())
                self.current_trait_node = ReflTraitNode(node, string_literal.spelling, ReflTraitNodeType.ENUM)
                return

        if kind == CursorKind.CLASS_DECL:
            if self.current_trait_node is not None:
                if self.current_trait_node.refl_node_type == ReflTraitNodeType.CLASS:
                    logger.info(f"Find reflection class {spelling}")
                    self.current_refl_node = ReflClass(node, self.current_trait_node)
                    self.current_refl_node_end_level = level

        if kind == CursorKind.STRUCT_DECL:
            if self.current_trait_node is not None:
                if self.current_trait_node.refl_node_type == ReflTraitNodeType.STRUCT:
                    logger.info(f"Find reflection struct {spelling}")
                    self.current_refl_node = ReflStruct(node, self.current_trait_node)
                    self.current_refl_node_end_level = level

        if kind == CursorKind.ENUM_DECL:
            if self.current_trait_node is not None:
                if self.current_trait_node.refl_node_type == ReflTraitNodeType.ENUM:
                    logger.info(f"Find reflection enum {spelling}")
                    self.current_refl_node = ReflEnum(node, self.current_trait_node)
                    self.current_refl_node_end_level = level

        if kind == CursorKind.FIELD_DECL:
            # 验证
            # 看看有没有PROPERTY ANNOTATION
            attr_node: Cursor = self.__find_attr_node(node)
            if attr_node is None:
                return
            attr_str: str = attr_node.spelling
            if not attr_str.startswith("Property"):
                logger.error(
                    f"类字段只能使用EPROPERTY修饰: node {self.current_refl_node.name}; type {self.current_refl_node.trait_node.refl_node_type}; filed {spelling}")
                self.has_error = True
                return
            prop_node = ReflProperty(node, attr_str)
            self.current_refl_node.append_property(prop_node)
            return

        if kind == CursorKind.CXX_METHOD:
            attr_node: Cursor = self.__find_attr_node(node)
            if attr_node is None:
                return
            attr_str: str = attr_node.spelling
            if not attr_str.startswith("Function"):
                logger.error(
                    f"类方法只能使用EFUNCTION修饰: node {self.current_refl_node.name}; type {self.current_refl_node.trait_node.refl_node_type}; filed {spelling}")
                self.has_error = True
                return
            if self.current_trait_node.refl_node_type == ReflTraitNodeType.STRUCT:
                logger.error("ESTRUCT里不能有EFUNECTION")
                self.has_error = True
                return
            func_node = ReflFunction(node, attr_str)
            self.current_refl_node.append_function(func_node)

        if kind == CursorKind.ENUM_CONSTANT_DECL:
            if self.current_trait_node is not None:
                if self.current_trait_node.refl_node_type != ReflTraitNodeType.ENUM:
                    logger.error("ECLASS ESTRUCT内不能声明枚举值")
                    self.has_error = True
                    return
                attr_node = self.__find_attr_node(node)
                if attr_node is not None:
                    attr_str: str = str(attr_node.spelling)
                    if not attr_str.startswith("Value"):
                        logger.error("枚举内的标注只能用EVALUE")
                        self.has_error = True
                        return
                    constant = ReflEnumConstant(node, attr_str)
                    self.current_refl_node.append_constant(constant)
                else:
                    constant = ReflEnumConstant(node)
                    self.current_refl_node.append_constant(constant)

        for child in node.get_children():
            self.ast_travel(child, level + 1)

        if level == self.current_refl_node_end_level:
            if self.current_refl_node is not None:
                if isinstance(self.current_refl_node, ReflClass):
                    self.result.append_class(self.current_refl_node)
                elif isinstance(self.current_refl_node, ReflStruct):
                    self.result.append_struct(self.current_refl_node)
                elif isinstance(self.current_refl_node, ReflEnum):
                    self.result.append_enum(self.current_refl_node)
                self.current_trait_node = None
                self.current_refl_node_end_level = -1
                self.current_refl_node = None


class AnnotationGenerator:
    @abstractmethod
    def match(self, entity, anno_key: str, anno_value: str) -> bool:
        ...

    @abstractmethod
    def generate_header(self, entity, anno_key: str, anno_value: str) -> str:
        ...

    @abstractmethod
    def generate_source(self, entity, anno_key: str, anno_value: str) -> str:
        ...


class ClassAnnotationGenerator(AnnotationGenerator):
    """
    生成时Struct和Class都会走这个生成器生成
    """

    @abstractmethod
    @override
    def match(self, entity: ReflClass | ReflStruct, anno_key: str, anno_value: str) -> bool:
        ...

    @abstractmethod
    @override
    def generate_header(self, entity: ReflClass | ReflStruct, anno_key: str, anno_value: str) -> str:
        ...

    @abstractmethod
    @override
    def generate_source(self, entity: ReflClass | ReflStruct, anno_key: str, anno_value: str) -> str:
        ...


class PropertyAnnotationGenerator(AnnotationGenerator):
    @abstractmethod
    def match(self, entity: ReflProperty, anno_key: str, anno_value: str) -> bool:
        pass

    @abstractmethod
    @override
    def generate_source(self, entity: ReflProperty, anno_key: str, anno_value: str) -> str:
        ...

    @override
    def generate_header(self, entity, anno_key: str, anno_value: str) -> str:
        return ""


class CodeGenerator:
    __class_annotation_generators: List[ClassAnnotationGenerator] = []
    __property_annotation_generators: List[PropertyAnnotationGenerator] = []

    @classmethod
    def append_class_annotation_generator(cls, gen_cls):
        instance = gen_cls()
        if isinstance(instance, ClassAnnotationGenerator):
            cls.__class_annotation_generators.append(instance)
        else:
            raise Exception("类标注生成器类型不符")

    @classmethod
    def append_property_annotation_generator(cls, gen_cls):
        instance = gen_cls()
        if isinstance(instance, PropertyAnnotationGenerator):
            cls.__property_annotation_generators.append(instance)
        else:
            raise Exception("类标注生成器类型不符")

    def __init__(self, ast_parse_result: ASTParseResult):
        if ast_parse_result is None:
            raise Exception("code generator needs valid AST parse result to generate code")
        self.result = ast_parse_result
        self.has_error = False

    def generate(self, out_path: Path, original_header_name: str):
        original_header_name = original_header_name.replace('.', '_')
        generated_header_codes = [f"""// This file is generated by ElbowEngine code generator for reflection
// 此文件由ElbowEngine代码生成器生成, 生成代码用于反射
// Don't edit this file manually
// 不要手动编辑此文件
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID {original_header_name}
"""]
        # 生成所有class
        for class_ in self.result.classes:
            generated_header_codes.append(rf"""#undef GENERATED_BODY_{original_header_name}_{class_.name}
#define GENERATED_BODY_{original_header_name}_{class_.name} \
public: \
    typedef ThisClass Super; \
    typedef {class_.name} ThisClass; \
    RTTR_ENABLE({class_.bases[0] if len(class_.bases) > 0 else ""}); \
    RTTR_REGISTRATION_FRIEND; \
private: \ """)
            for key, value in class_.attr_map.items():
                if key == "":
                    continue
                if key == "Name":
                    continue
                valid = False
                for anno_gen in self.__class_annotation_generators:
                    if anno_gen.match(class_, key, value):
                        anno_code = anno_gen.generate_header(class_, key, value)
                        generated_header_codes.append(anno_code)
                        valid = True
                if not valid and key != "__fixed_item__":
                    self.has_error = True
                    logger.error(f"无法将标注{key}:{valid}应用在{class_.full_name}上")
                    return
            generated_header_codes.append(";")

        # 生成所有struct
        for struct in self.result.structs:
            generated_header_codes.append(f"""#undef GENERATED_BODY_{original_header_name}_{struct.name} \\
#define GENERATED_BODY_{original_header_name}_{struct.name} \\
RTTR_REGISTRATION_FRIEND \\
typedef {struct.name} ThisStruct; \\""")
            if len(struct.bases) != 0:
                generated_header_codes.append(f"typedef {struct.bases[0]} Super; \\\nRTTR_ENABLE({struct.bases[0]}); \\")
            for key, value in struct.attr_map.items():
                valid = False
                if key == "":
                    continue
                if key == "Name":
                    continue
                for anno_gen in self.__class_annotation_generators:
                    if anno_gen.match(struct, key, value):
                        anno_code = anno_gen.generate_header(struct, key, value)
                        generated_header_codes.append(anno_code)
                        valid = True
                if not valid and key != "__fixed_item__":
                    self.has_error = True
                    logger.error(f"无法将标注{key}:{valid}应用在{struct.full_name}上")
                    return
            generated_header_codes.append(";")

        # 生成所有enum
        for enum in self.result.enums:
            ...  # enum不需要GENERATED_BODY()

        # 生成所有class和struct对应的property
        for class_ in self.result.classes + self.result.structs:
            class_name = class_.full_name
            if "Name" in class_.attr_map.keys():
                class_name = class_.attr_map["Name"]
            generated_header_codes.append(f"""#undef GENERATED_SOURCE_{original_header_name}_{class_.name} \\
#define GENERATED_SOURCE_{original_header_name}_{class_.name} \\
rttr::registration::class_<{class_.full_name}>(\"{class_name}\").constructor<>()(rttr::policy::ctor::as_raw_ptr) \\""")
            for property_ in class_.properties:
                property_name = property_.name
                if "Name" in property_.attr_map.keys():
                    property_name = property_.attr_map["Name"]
                prop_str = f".property(\"{property_name}\", &{class_.full_name}::{property_.name})"
                for key, value in property_.attr_map.items():
                    if key == "":
                        continue
                    if key == "Name":
                        continue
                    valid = False
                    for gen in self.__property_annotation_generators:
                        if gen.match(property_, key, value):
                            valid = True
                            prop_str += gen.generate_source(property_, key, value)
                    if not valid and key != "__fixed_item__":
                        self.has_error = True
                        logger.error(f"标注 {key}:\"{value}\" 无法应用在属性{class_.full_name}::{property_.name}上")
                        return
                prop_str += " \\"
                generated_header_codes.append(prop_str)
            generated_header_codes.append(";")

        # 生成Enum对应的source
        for enum in self.result.enums:
            enum_name = enum.full_name
            if "Name" in enum.attr_map.keys():
                enum_name = enum.attr_map["Name"]
            generated_header_codes.append(f"""#undef GENERATED_SOURCE_{original_header_name}_{enum.name} \\
#define GENERATED_SOURCE_{original_header_name}_{enum.name} \\
rttr::registration::enumeration<{enum.full_name}>(\"{enum_name}\")(""")
            for idx, value in enumerate(enum.constants):
                enum_value_name = value.name
                if "Name" in value.attr_map.keys():
                    enum_value_name = value.attr_map["Name"]
                if idx != len(enum.constants) - 1:
                    generated_header_codes.append(f'value("{enum_value_name}", {enum.full_name}::{value.name}), \\')
                else:
                    generated_header_codes.append(f'value("{enum_value_name}", {enum.full_name}::{value.name}) \\')
            generated_header_codes.append("); \\")
            generated_header_codes.append(";")

        # 将generated_source内容整合到一起
        generated_header_codes.append(f'''#undef GENERATED_SOURCE_{original_header_name}
#define GENERATED_SOURCE_{original_header_name} \\''')
        for class_ in self.result.classes + self.result.structs:
            generated_header_codes.append(f"GENERATED_SOURCE_{original_header_name}_{class_.name} \\")
        for enum in self.result.enums:
            generated_header_codes.append(f"GENERATED_SOURCE_{original_header_name}_{enum.name} \\")
        generated_header_codes.append(";")

        return '\n'.join(generated_header_codes)


def class_annotation_generator(cls):
    CodeGenerator.append_class_annotation_generator(cls)
    return cls


def property_annotation_generator(cls):
    CodeGenerator.append_property_annotation_generator(cls)
    return cls
