import json
from argparse import ArgumentParser
from pathlib import Path
import subprocess
from datetime import datetime
from typing import Any


# 配置文件保存文件最后一次修改时间，使用json


def find_all_header_files(my_path: Path):
    return [my_file for my_file in my_path.rglob('*.h')]


def check_updated(file_path: Path, output_path: Path, cache: Any):
    """
    检查是否有文件更新
    :return:
    """
    cached_modify_time = cache.get(str(file_path), "1980/01/01 00:00:00")
    cached_modify_time = datetime.strptime(cached_modify_time, "%Y/%m/%d %H:%M:%S")

    file_modify_time = datetime.fromtimestamp(file_path.stat().st_mtime).strftime("%Y/%m/%d %H:%M:%S")
    file_modify_time = datetime.strptime(file_modify_time, "%Y/%m/%d %H:%M:%S")
    if file_modify_time > cached_modify_time:
        cache[str(file_path)] = file_modify_time.strftime("%Y/%m/%d %H:%M:%S")
        return True
    return False


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('--output-files-path', type=str, required=True)
    parser.add_argument('--reflection-tool-path', type=str, required=True)
    parser.add_argument('--process-files-path', type=str, nargs="*", required=True)
    # 自己工程的头文件路径 递归添加每一个文件夹
    parser.add_argument('--include-path', type=str, nargs="*", required=True)
    parser.add_argument('--config-path', type=str, required=True)
    args = parser.parse_args()

    output_files_path = args.output_files_path
    reflection_tool_path = args.reflection_tool_path
    process_files_path = args.process_files_path
    include_paths = args.include_path
    cache_file = Path(output_files_path) / "cache.json"
    config_path = args.config_path

    Path(output_files_path).mkdir(parents=True, exist_ok=True)

    if cache_file.exists():
        with open(cache_file, "r") as f:
            cache = json.load(f)
    else:
        cache = {}

    files_to_process = []
    for path in process_files_path:
        files_to_process.extend(find_all_header_files(Path(path)))

    third_party_paths = []

    my_include_paths = []
    for include_path in include_paths:
        my_include_paths.append(f"--inc={include_path}")

    command = [reflection_tool_path, "", "--output-file-path=" + output_files_path]
    command.extend(my_include_paths)
    config_command = [f"--config={str(config_path)}"]
    command.extend(config_command)
    command.append("--")

    processes = []
    for file in files_to_process:
        command[1] = str(file)
        if check_updated(file, Path(output_files_path), cache):
            # if True:
            print(subprocess.list2cmdline(command))
            proc = subprocess.Popen(command)
            processes.append(proc)

    for process in processes:
        process.wait()

    print(f"Request process {len(files_to_process)} files, process {len(processes)} files eventually.")
    with open(cache_file, "w") as f:
        json.dump(cache, f, indent=4)
