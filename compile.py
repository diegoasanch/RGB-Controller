import re
import subprocess
import sys
import os

BOARD = "esp8266:esp8266:d1_mini_clone"
BUILD_DIR = "./dist"
VERSION_DIR = os.path.join(BUILD_DIR, "./version")
SKETCH_PATH = "./src/src.ino"
BIN_FILE = 'src.ino.bin'

version_regex = re.compile(r'^v\d+\.\d+\.\d+(-rc(\d+)?)?$')

def compile_sketch(board: str, build_dir: str, sketch_path: str):
    COMPILE_CMD = f"arduino-cli compile -b {board} -e --output-dir {build_dir} --no-color {sketch_path}"

    print("Compiling...")
    with subprocess.Popen(COMPILE_CMD, stdout=subprocess.PIPE, shell=True) as proc:
        print(proc.stdout.read())
        print("Compilation done, result:")

def rename_build(new_name: str):
    os.rename(
        os.path.join(BUILD_DIR, BIN_FILE),
        os.path.join(VERSION_DIR, new_name + ".bin")
    )

def is_name_valid(name: str) -> bool:
    return bool(re.match(version_regex, name))

def main():
    compile_sketch(BOARD, BUILD_DIR, SKETCH_PATH)

    # TODO: Validate name does not exist before compiling
    args = sys.argv[1:]
    if len(args) > 0:
        version = args[0]
        if is_name_valid(version):
            rename_build(version)
        else:
            print(f"Invalid version name {version}")
            print("Name must be a valid semantic versioning string")


if __name__ == '__main__':
    main()
