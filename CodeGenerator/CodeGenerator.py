import argparse
import sys
import typing
from pathlib import Path
from clang.cindex import Config, CursorKind, Cursor, Index


def parse_meta(meta: str):
    metas = meta.strip().split(sep=',')
    metas.remove("Reflected")
    rtn_metas = {}
    for meta_value in metas:
        if '=' in meta_value:
            name, value = meta_value.split('=')
            value = value.strip().strip("\"").strip()
            if name.strip() != "":
                rtn_metas[name.strip()] = value
        else:
            if meta_value.strip() != "" and meta_value.strip() != "Name":
                rtn_metas[meta_value.strip()] = True
    return rtn_metas


def print_ast(cursor, indent=''):
    print(f"{indent}{cursor.kind} : {cursor.spelling}")
    for child in cursor.get_children():
        print_ast(child, indent + '  ')


class ReflectedField:
    def __init__(self, cursor: Cursor, meta: str):
        self.cursor = cursor
        self.meta = meta
        self.metas = None

    def __str__(self):
        return f'ReflectedField[name={self.cursor.displayname} metas={self.metas}]'

    def __repr__(self):
        return f'ReflectedField[name={self.cursor.displayname} metas={self.metas}]'

    def parse_meta(self):
        self.metas = parse_meta(self.meta)


class ReflectedClass:
    def __init__(self, cursor: Cursor):
        self.metas = None
        self.class_cursor: Cursor = cursor
        self.fields = []
        self.need_reflect = False
        self.meta: str = ""
        self.line_number = 0

    def __str__(self):
        return f"ReflectedClass[need_reflect={self.need_reflect} meta={self.meta} fields={self.fields} metas={self.metas}]"

    def __repr__(self):
        return f"ReflectedClass[need_reflect={self.need_reflect} meta={self.meta} fields={self.fields} metas={self.metas}]"

    def parse_meta(self):
        self.metas = parse_meta(self.meta)
        [i.parse_meta() for i in self.fields]

    def get_parent_class(self):
        for cls in self.class_cursor.get_children():
            if cls.kind == CursorKind.CXX_BASE_SPECIFIER:
                return cls.displayname
        return ""

    def fully_qualified_name(self):
        return self._fully_qualified(self.class_cursor)

    def _fully_qualified(self, c: Cursor):
        """
        获取c所指的全称
        例:
        namespace Test {
        class A;
        }
        对A的Cursor调用此函数返回Test_A
        :param c:
        :return:
        """
        if c is None:
            return ''
        elif c.kind == CursorKind.TRANSLATION_UNIT:
            return ''
        else:
            res = self._fully_qualified(c.semantic_parent)
            if res != '':
                return res + '_' + c.spelling
        return c.spelling


class CppHeaderParser:
    def __init__(self, clang_lib_path: str, filepath: str, gen_path: str):
        self.file: Path = Path(filepath)
        Config.set_library_file(clang_lib_path)
        self.index = Index.create()
        self.transition_unit = self.index.parse(str(self.file), args=["-xc++"])
        self.classes: typing.List[ReflectedClass] = []
        self.path = Path(gen_path)
        if not self.path.is_dir():
            print("please enter a folder path.", file=sys.stderr)
            exit(1)
        self.generated_file_name: str = ""
        self.current_file_id: str = ""

    def generate_current_file_id(self, cursor: Cursor):
        if self.current_file_id != "":
            return
        if cursor.kind == CursorKind.TRANSLATION_UNIT:
            return
        current_file_path = Path(cursor.location.file.name).name
        if current_file_path == "Object.h":
            return
        self.current_file_id = str(current_file_path).replace(".", "_")

    def process_class_decl(self, cursor: Cursor):
        for c in cursor.get_children():
            c_cursor: Cursor = c
            meta_str = c_cursor.displayname
            if c_cursor.kind == CursorKind.ANNOTATE_ATTR and "Reflected" in meta_str:
                return True, meta_str
        return False, "None"

    def get_attr_line(self, attr_cursor: Cursor):
        return attr_cursor.location.line

    def process_filed_decl(self, cursor: Cursor):
        """
        处理字段的Reflected声明 如果字段被声明了类却没有则报错并结束
        :param cursor: 字段的cursor
        :return:
        """

        def is_refl_field_valid(cursor: Cursor):
            """
            检验这个字段是不是有效的 如果被声明反射而类没有则无效
            :param cursor:
            :return:
            """
            if cursor.kind != CursorKind.FIELD_DECL:
                return False
            parent_cursor = cursor.semantic_parent
            for cur in parent_cursor.get_children():
                my_cur: Cursor = cur
                meta_str = str(my_cur.displayname)
                if my_cur.kind == CursorKind.ANNOTATE_ATTR and "Reflected" in meta_str:
                    return True
            return False

        if not is_refl_field_valid(cursor):
            print(f'{cursor.location}: field with Reflected must declared in class with Reflected', file=sys.stderr)
            exit(1)

        for c in cursor.get_children():
            c_cursor: Cursor = c
            meta_str = str(c_cursor.displayname)
            if c_cursor.kind == CursorKind.ANNOTATE_ATTR and "Reflected" in meta_str:
                # 查看父类有没有被标记Reflected
                return True, ReflectedField(cursor, meta_str)
        return False, None

    def traverse_ast(self, cursor: Cursor):
        self.generate_current_file_id(cursor)
        my_cursor: Cursor = cursor
        if my_cursor.kind == CursorKind.CLASS_DECL:
            # 找到类声明
            my_class = ReflectedClass(my_cursor)
            for cc in my_cursor.get_children():
                if cc.kind == CursorKind.FIELD_DECL:
                    # 看类的字段需不需要反射
                    need_refl, refl_field = self.process_filed_decl(cc)
                    if need_refl:
                        my_class.fields.append(refl_field)

                if cc.kind == CursorKind.ANNOTATE_ATTR:
                    # 看这个类需不需要反射
                    need_refl_class, class_meta = self.process_class_decl(my_cursor)
                    my_class.line_number = self.get_attr_line(cc)
                    if need_refl_class:
                        my_class.need_reflect = True
                        my_class.meta = class_meta

            if my_class.need_reflect:
                # 需要反射就加入反射列表
                self.classes.append(my_class)

        for child in cursor.get_children():
            self.traverse_ast(child)

    def parse_meta(self):
        [i.parse_meta() for i in self.classes]

    def generate_filed_code(self, all_fields: list[ReflectedField], class_name: str):
        rtn_str = ""
        for field in all_fields:
            field_decl_name = field.cursor.displayname
            field_name = field.cursor.displayname if "Name" not in field.metas else field.metas["Name"]
            rtn_str += f'    .property("{field_name}", &{class_name}::{field_decl_name})'
            for meta_key, meta_value in field.metas.items():
                if meta_key != "Name":
                    rtn_str += f'(rttr::metadata("{meta_key}", "{meta_value}"))'
            rtn_str += '\\\n'
        return rtn_str

    def generate_class_code(self):
        class_code = ""
        for class_ in self.classes:
            class_name = class_.class_cursor.displayname
            generate_code_a = f"""
#define GENERATED_CODE_{class_.line_number + 1} \\
{{\\
rttr::registration::class_<{class_name}>("{class_name}")\\
{self.generate_filed_code(class_.fields, class_name)};\\
}}
"""
            generate_code_macro = f"""
#define GENERATED_BODY_INTERNAL_{self.current_file_id}_{class_.line_number + 1} \\
public:\\
    typedef {class_.get_parent_class()} Super;\\
    typedef {class_.fully_qualified_name()} ThisClass;\\
    {"RTTR_ENABLE(" + class_.get_parent_class() + ")" if class_.get_parent_class() != "" else None}\\
    RTTR_REGISTRATION_IN_CLASS({class_.fully_qualified_name()})\\
    GENERATED_CODE_{class_.line_number + 1}
"""
            class_code = class_code + generate_code_a + generate_code_macro
        return class_code

    def generate(self):
        # 生成代码字符串
        code = '// this file is generated by code generator, do not modify\n'
        file_macro = self.generated_file_name.replace(".", "_").upper()
        code += f'#ifdef {file_macro}\n'
        code += f'#error "generated file for {self.file.name} has been included!"\n'
        code += '#endif\n'
        code += f'#ifndef {file_macro}\n'
        code += f'#define {file_macro}\n'
        gen_code = self.generate_class_code()
        if gen_code == "":
            # 没有需要生成的类
            exit(1)
        code += gen_code
        code += "#undef CURRENT_FILE_ID\n"
        code += f"#define CURRENT_FILE_ID {self.current_file_id}\n"
        code += '#endif\n'
        with Path(self.generated_file_name).open('w') as f:
            f.write(code)

    def process(self):
        input_path = Path(self.transition_unit.spelling)
        name = input_path.name
        if ".generated.h" in name:
            return
        self.generated_file_name = name.replace(".h", ".generated.h")
        self.traverse_ast(self.transition_unit.cursor)
        self.parse_meta()
        self.generate()

class FileWatcher:
    """
    检测文件是否有变更
    """
    def __init__(self):
        ...


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Code Generator')
    parser.add_argument('--libpath', type=str, help='Path to the clang library', required=True)
    parser.add_argument('--file', type=str, help='File path to parser and generate code', required=True)
    parser.add_argument('--genpath', type=str, help='File path to generated code', required=True)

    clang_lib_path = parser.parse_args().libpath
    file_path = parser.parse_args().file
    gen_path = parser.parse_args().genpath
    # TODO 文件缓存 多线程处理
    cpp_parser = CppHeaderParser(clang_lib_path, file_path, gen_path)
    cpp_parser.process()
