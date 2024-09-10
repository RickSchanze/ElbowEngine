import functools
import hashlib
import json
import multiprocessing
from typing import override, List, Dict
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor

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


class FileProcess:
    class ProcessResult:
        def __init__(self, src: Path, dst: Path):
            self.src = src
            self.dst = dst

    def __init__(self, working_dir: str, folders: List[str], max_process_nums: int):
        self.working_dir = Path(working_dir)
        self.folders = folders
        self.files_need_to_process: List[FileProcess.ProcessResult] = []
        self.file_hash_caches: Dict[str, str] = {}
        self.max_process_num = max_process_nums

    @staticmethod
    def hash(path: Path):
        with open(path, "rb") as f:
            data = f.read()
        return hashlib.md5(data).hexdigest()

    def calculate_files_need_process_folder(self, result: ProcessResult):
        new_hash = FileProcess.hash(result.src)
        old_hash = self.file_hash_caches.get(result.src.as_posix())
        if old_hash != new_hash:
            self.file_hash_caches[result.src.as_posix()] = new_hash
            self.files_need_to_process.append(result)

    def calculate_all_files_need_process(self):
        cache_path = self.working_dir / "Generated" / "Cache.json"
        if not cache_path.exists():
            cache_path.open('w').close()
            self.file_hash_caches = {}
        else:
            try:
                self.file_hash_caches = json.load(cache_path.open('r'))
            except Exception as e:
                logger.warn(f"读取json文件哈希缓存{cache_path}失败, 异常: {e}")
                self.file_hash_caches = {}
        # TODO: 计算文件哈希的操作应该可以多进程化
        for folder in self.folders:
            folder_path = self.working_dir / folder
            all_h_files = folder_path.rglob("*.h")
            for h_file in all_h_files:
                new_result = FileProcess.ProcessResult(h_file, self.working_dir / "Generated" / folder / (h_file.name[:-2] + ".generated.h"))
                self.calculate_files_need_process_folder(new_result)
        with cache_path.open('w') as f:
            json.dump(self.file_hash_caches, cache_path.open('w'))


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
    process = FileProcess(ElbowEngineCodeGenConfig.working_dir, ElbowEngineCodeGenConfig.process_folder, ElbowEngineCodeGenConfig.max_process_count)
    process.calculate_all_files_need_process()
