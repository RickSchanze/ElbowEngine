import re
from typing import override
from pathlib import Path
from clang.cindex import Index

from Config import ElbowEngineCodeGenConfig
from CodeGenerator import ReflStruct, property_annotation_generator, PropertyAnnotationGenerator, ReflProperty
from CodeGenerator import logger, ASTParser, class_annotation_generator, ClassAnnotationGenerator, ReflClass, CodeGenerator


@class_annotation_generator
class NonCopyableGenerator(ClassAnnotationGenerator):
    def generate_source(self, entity: ReflClass | ReflStruct, anno_key: str, anno_value: str) -> str:
        return ""

    @override
    def generate_header(self, class_: ReflClass | ReflStruct, anno_key: str, anno_value: str) -> str:
        return f"public:\\ \n{class_.name}({class_.name}&) = delete; \\\n{class_.name}& operator=({class_.name}&) = delete; \\"

    @override
    def match(self, entity, anno_key: str, anno_value: str) -> bool:
        if anno_key == "NonCopyable":
            return True
        return False


@property_annotation_generator
class SimpleEditorMetaGenerator(PropertyAnnotationGenerator):
    def generate_source(self, entity: ReflProperty, anno_key: str, anno_value: str) -> str:
        return f" EDITOR_META( (rttr::metadata(\"{anno_key}\", \"{anno_value}\") )"

    def match(self, entity: ReflProperty, anno_key: str, anno_value: str) -> bool:
        if anno_key in ["Label"]:
            return True
        return False


@property_annotation_generator
class ReferencedTypeGenerator(PropertyAnnotationGenerator):

    def match(self, entity: ReflProperty, anno_key: str, anno_value: str) -> bool:
        if entity.type_name in ElbowEngineCodeGenConfig.referenced_type:
            return True
        return False

    def generate_source(self, entity: ReflProperty, anno_key: str, anno_value: str) -> str:
        return "(rttr::policy::as_reference_wrapper)"


def run():
    index = Index.create()
    logger.info(r"Begin Prcocess file 'C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine\CodeGen\a.cpp'")
    translation_unit = index.parse(r'C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine\CodeGen\a.cpp')
    traverser = ASTParser()
    traverser.ast_travel(translation_unit.cursor)
    gen = CodeGenerator(traverser.get_result())
    res = gen.generate(Path(r"C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine\CodeGen\generated\a.cpp"), "a.cpp")
    print(res)
    logger.info(r"End Prcocess file 'C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine\CodeGen\a.cpp'")


if __name__ == '__main__':
    print(re.search(r"/ThirdParty/", "C:/ThirdParty/a.h"))
