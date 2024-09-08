import re
from typing import List
from pathlib import Path


class FileProcessResult:
    def __init__(self, source: Path, working_dir: Path, file: Path):
        self.working_dir: Path = working_dir
        self.source_dir: Path = source
        self.source_file: Path = file
        self.target_dir: Path
        self.target_file: Path


class FileChunk:
    def __init__(self):
        self.chunk_top: Path | None = None
        self.chunk_files: List[Path] = []


class FileProcessor:
    def __init__(self, exclude: List[str], work_dir: Path):
        self.exclude = exclude if exclude else []
        self.work_dir = work_dir
        if not self.work_dir.is_dir():
            raise Exception(f"工作目录必须是一个存在的路径! {str(work_dir.resolve())}")

    def has_match_exclude(self, string: str):
        has_match = False
        for exclude in self.exclude:
            if re.search(exclude, string):
                has_match = True
        return has_match

    def get_pending_files(self) -> List[FileProcessResult]:
        """
        获取所有需要处理的文件列表
        加入现在文件目录是
        --ElbowEngine
        ----CodeGen
        ----Content
        ----Engine
        ------Core
        --------CMakeLists.txt
        ------Tool
        --------CMakeLists.txt
        ------CMakeLists.txt
        ----Generated
        ----CMakeList.txt
        传入work_dir是ElbowEngine的话
        会递归寻找ElbowEngine中包含CMakeLists.txt的文件夹,且该文件夹下面有.h文件的文件夹，并返回一个列表
        列表里的元素包括：
          1. 一个表示绝对路径的Path
          2. 一个相对于work_dir的Path
          3. 一个文件绝对路径的Path
          4. 一个生成文件应该放置的Path
             例如上面对于ElbowEngine/Engine/Core的文件 work_dir为ElbowEngin
             则这个路径是ElbowEngine/Generated/Engine/Core/
        :return:
        """
        # 找到所有符合要求的文件夹
        folders_with_cmake = [folder for folder in self.work_dir.rglob('CMakeLists.txt') if folder.is_file()]
        folders_with_cmake_filtered = []
        for folder in folders_with_cmake:
            has_match = False
            for filter_ in self.exclude:
                if re.search(filter_, str(folder.as_posix())) is not None:
                    has_match = True
                    break
            if not has_match:
                folders_with_cmake_filtered.append(folder.parent)

        # 获取所有文件
        file_chunks: List[FileChunk] = []
        for folder in folders_with_cmake_filtered:
            chunk = FileChunk()
            chunk.chunk_top = folder
            files = []
            for path in folder.iterdir():
                if path in folders_with_cmake_filtered:
                    continue
                if path.name.endswith(".h"):
                    files.append(path)
                files += list(path.rglob("*.h"))
            for file in files:
                if self.has_match_exclude(file.resolve().as_posix()):
                    continue
                chunk.chunk_files.append(file)
            file_chunks.append(chunk)


if __name__ == '__main__':
    f = FileProcessor(["/ThirdParty/", '/cmake-build-([\\s\\S]*)/', "/Generated/"], Path(r"C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine"))
    f.get_pending_files()
