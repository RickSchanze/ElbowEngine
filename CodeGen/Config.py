import logging


class ElbowEngineCodeGenConfig:
    # clang的dll
    lib_clang_path: str = r"C:\Users\Echo\Software\LLVM\bin\libclang.dll"
    # 日志输出等级
    debug_level: int = logging.INFO
    # 使用std::reference_wrapper的类型
    referenced_type = [
        "TArray", "std::vector",
        "TStaticArray", "std::array",
        "TMap", "std::map",
        "THashMap", "std::unordered_map",
    ]
    # 这里写ElbowEngine的位置
    working_dir = r"C:\Users\Echo\SyncWork\Work\Projects\ElbowEngine"
    # 相对于ElbowEngine的文件夹位置 开头不要写"/"
    process_folder = ["Engine/Core", "Engine/Function", "Engine/Platform", "Engine/Resource", "Engine/Tool"]
    # 进程池并行处理最大数量
    max_process_count = 10
