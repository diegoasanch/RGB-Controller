import re
import subprocess
import sys
import os

BOARD = 'esp8266:esp8266:d1_mini_clone'

CWD = os.getcwd()
BUILD_DIR = os.path.join(CWD, 'dist')
VERSION_DIR = os.path.join(BUILD_DIR, 'version')
SKETCH_PATH = os.path.join(CWD, 'src','src.ino')
BIN_FILE = 'src.ino.bin'

version_regex = re.compile(r'^v\d+\.\d+\.\d+(-rc(\d+)?)?$')

def compile_sketch(board: str, build_dir: str, sketch_path: str):
    COMPILE_CMD = f'arduino-cli compile -b {board} -e --output-dir {build_dir} {sketch_path}'

    print('Compiling...')
    with subprocess.Popen(COMPILE_CMD, stdout=subprocess.PIPE, shell=True) as proc:
        print('> Compilation done, result:\n')
        print(proc.stdout.read().decode("utf-8"))
        return proc.returncode == 0

def rename_build(new_name: str):
    if not os.path.exists(VERSION_DIR):
        os.makedirs(VERSION_DIR)
    os.rename(
        os.path.join(BUILD_DIR, BIN_FILE),
        os.path.join(VERSION_DIR, new_name + '.bin')
    )

def is_version_valid(name: str) -> bool:
    return bool(re.match(version_regex, name))

def version_exists(name: str) -> bool:
    return os.path.exists(os.path.join(VERSION_DIR, name + '.bin'))

def yes_or_no(question: str) -> bool:
    while True:
        answer = input(question + ' (y/n): ')
        if answer == 'y':
            return True
        elif answer == 'n':
            return False
        else:
            print('Invalid answer')

def main():
    cli_args = sys.argv[1:]
    will_rename = False
    version = ''

    if len(cli_args) > 0:
        version = cli_args[0]
        if not is_version_valid(version):
            print(f'Invalid version name {version}')
            print('Name must be a valid semantic versioning string')
            sys.exit(1)

        will_rename = True

        if version_exists(version):
            print(f'Version {version} already exists')
            print('Do you want to overwrite it?')
            if not yes_or_no(f'Overwrite {version}?'):
                sys.exit(0)

    status_code = compile_sketch(BOARD, BUILD_DIR, SKETCH_PATH)

    if status_code != 0:
        print("Compilation failed, exiting...")
        sys.exit(status_code)

    if will_rename:
        print(f'Renaming build to {version}')
        rename_build(version)


if __name__ == '__main__':
    main()
