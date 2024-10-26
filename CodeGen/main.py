import argparse
import hashlib
import json
import logging
import multiprocessing
import os
from logging.handlers import QueueHandler
from typing import override, List, Dict
from pathlib import Path

import clang.cindex
from clang.cindex import Index

from CodeGenerator import log_queue
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
        return f" EDITOR_META(rttr::metadata(\"{anno_key}\", \"{anno_value}\"))"

    def match(self, entity: ReflProperty, anno_key: str, anno_value: str) -> bool:
        if anno_key in ["Label"]:
            return True
        return False


@property_annotation_generator
class DummyGenerator(PropertyAnnotationGenerator):
    """
    用于特殊的不需要匹配的标注
    """
    def generate_source(self, entity: ReflProperty, anno_key: str, anno_value: str) -> str:
        return ""

    def match(self, entity: ReflProperty, anno_key: str, anno_value: str) -> bool:
        if anno_key in ["Getter", "Setter"]:
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

    def append_file_if_need_process(self, result: ProcessResult):
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
                logger.warning(f"读取json文件哈希缓存{cache_path}失败, 异常: {e}")
                self.file_hash_caches = {}
        # TODO: 计算文件哈希的操作应该可以多进程化
        for folder in self.folders:
            folder_path = self.working_dir / folder
            all_h_files = folder_path.rglob("*.h")
            for h_file in all_h_files:
                new_result = FileProcess.ProcessResult(h_file, self.working_dir / "Generated" / (h_file.name[:-2] + ".generated.h"))
                self.append_file_if_need_process(new_result)

    def process_one(self, result: ProcessResult):
        # 配置log
        handler = QueueHandler(log_queue)
        log = logging.getLogger()
        log.setLevel(ElbowEngineCodeGenConfig.debug_level)
        log.addHandler(handler)

        args = [f"-I{(self.working_dir / folder).as_posix()}" for folder in self.folders]
        args += [f"-I{(self.working_dir / folder).as_posix()}" for folder in ElbowEngineCodeGenConfig.extra_include_folder]
        args.append("-DREFLECTION")
        args += ElbowEngineCodeGenConfig.clang_args
        index = Index.create()
        args.append(f"-I{result.dst.parent.as_posix()}")
        log.info(f"[{os.getpid()}] 开始解析文件 {result.src.as_posix()}")
        tu = index.parse(result.src.as_posix(), args)
        parse_error = False
        for diag in tu.diagnostics:
            if diag.severity == clang.cindex.Diagnostic.Error or diag.severity == clang.cindex.Diagnostic.Fatal:
                parse_error = True
                log.error(f"[{os.getpid()}] 解析{result.src}:{diag.location.line}:{diag.location.column}出错: {diag.spelling}")
        if parse_error:
            return -1
        traverser = ASTParser(result.src, log)
        traverser.ast_travel(tu.cursor)
        if traverser.has_error:
            logger.error(f"[{os.getpid()}] 解析{result.src}失败")
            return -1
        gen = CodeGenerator(traverser.get_result())
        if len(gen.result.classes) <= 0 and len(gen.result.structs) <= 0 and len(gen.result.enums) <= 0:
            logger.info(f"[{os.getpid()}] 解析{result.src}结束, 未找到需要反射的struct/class/enum")
            return 0
        res = gen.generate(result.src.name)
        if gen.has_error:
            logger.error(f"[{os.getpid()}] 解析{result.src}失败")
            return -1
        try:
            result.dst.parent.mkdir(parents=True, exist_ok=True)
            with open(result.dst, "w", encoding='utf-8') as f:
                f.write(res)
        except Exception as e:
            logger.exception(e)
            return -1
        log.info(f"[{os.getpid()}] 解析文件 {result.src.as_posix()} 结束")
        return 0

    def process_all(self):
        listener = logging.handlers.QueueListener(log_queue, logging.StreamHandler())
        listener.start()
        # results = [0]
        # for one in self.files_need_to_process:
        #     self.process_one(one)
        with multiprocessing.Pool(ElbowEngineCodeGenConfig.max_process_count) as pool:
            results = pool.map(self.process_one, self.files_need_to_process)
            # pool.close()
            # pool.join()
        if all(result == 0 for result in results):
            logger.info("所有文件处理完成")
            with open(self.working_dir / "Generated" / "Cache.json", "w") as f:
                json.dump(self.file_hash_caches, f)
        else:
            logger.error("某些进程未能正确执行")
        listener.stop()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="process header file, generate code")
    parser.add_argument('--includes', nargs='+', help='include file')
    args = parser.parse_args()
    files = args.includes
    ElbowEngineCodeGenConfig.extra_include_folder += [f"-I{file}" for file in files]
    process = FileProcess(ElbowEngineCodeGenConfig.working_dir, ElbowEngineCodeGenConfig.process_folder, ElbowEngineCodeGenConfig.max_process_count)
    process.calculate_all_files_need_process()
    process.process_all()
