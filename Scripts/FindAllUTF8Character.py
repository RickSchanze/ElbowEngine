import os
from pathlib import Path

from ScriptGlobalConfig import *

if __name__ == '__main__':
    text_extensions = (
        '.txt', '.csv', '.json', '.xml',
        '.yaml', '.html', '.js', '.css',
        '.py', '.java', '.c', '.cpp', ".h", ".hpp"
    )
    all_characters = set([])
    for file_path in Path(PROJECT_CODE_PATH).rglob("*"):
        if Path.is_file(file_path):
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    for line_num, line in enumerate(f, 1):
                        for col_num, char in enumerate(line, 1):
                            all_characters.add(char)
            except UnicodeDecodeError:
                print(f"跳过非UTF-8文件: {file_path}")
            except Exception as e:
                print(f"处理文件 {file_path} 时出错: {str(e)}")
    all_characters.remove('\n')
    all_characters.remove('\\')
    all_characters.add('\\\\')
    all_characters.remove('"')
    all_characters.add('\\"')
    all_characters.add('qwertyuiopasfdhjklghdlskasdjmznxbv')
    with open(IMGUI_USED_TEXT_OUTPUT_FILE, 'w', encoding='utf-8') as f:
        f.write("// 此文件由Python脚本自动生产\n")
        f.write("#pragma once\n")
        output_str = "".join(item for item in all_characters)
        f.write(f'#define IMGUI_USED_CHARACTER_CODE \\\n')
        f.write(f'"{output_str}"')
