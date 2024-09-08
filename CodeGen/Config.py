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
    # 一系列正则表达式，能匹配上这个正则表达式的文件不会被处理
    excluded_filter = [
        r"/ThirdParty/"
    ]
