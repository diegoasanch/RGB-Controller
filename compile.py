import re
import subprocess
import sys
import os

BOARD = 'esp8266:esp8266:d1_mini_clone'

CWD = os.getcwd()
BUILD_DIR = os.path.join(CWD, 'build')
OUT_DIR = os.path.join(BUILD_DIR, 'bin')
SKETCH_PATH = os.path.join(CWD, 'src','src.ino')
BIN_FILE = 'src.ino.bin'

version_regex = re.compile(r'^v\d+\.\d+\.\d+(-rc(\d+)?)?$')

def compile_sketch(board: str, build_dir: str, sketch_path: str):
    COMPILE_CMD = f'arduino-cli compile -b {board} -e --output-dir {build_dir} {sketch_path}'

    print('Compiling...')
    with subprocess.Popen(COMPILE_CMD, stdout=subprocess.PIPE, shell=True) as proc:
        output = proc.stdout.read()
        print('> Compilation done, result:\n')
        print(output.decode("utf-8"))
        return proc.returncode

def rename_build(new_name: str):
    if not os.path.exists(OUT_DIR):
        os.makedirs(OUT_DIR)
    os.rename(
        os.path.join(BUILD_DIR, BIN_FILE),
        os.path.join(OUT_DIR, new_name+'.bin')
    )

def is_version_valid(name: str) -> bool:
    return bool(re.match(version_regex, name))

def version_exists(name: str) -> bool:
    return os.path.exists(os.path.join(OUT_DIR, name + '.bin'))

def yes_or_no(question: str) -> bool:
    while True:
        answer = input(question + ' (y/n): ').lower()
        if answer == 'y':
            return True
        elif answer == 'n':
            return False
        else:
            print('Invalid answer')

def write_config_version(version: str):
    'Writes version to config.h'
    with open(os.path.join('src', 'config.h'), 'r+') as f:
        lines = list(f.readlines())
        version_line = find_version_line(lines)

        if version_line == -1:
            print('Failed to write version to config.h')
            sys.exit(1)

        lines[version_line] = f'    const String VERSION = "{version}";\n'

        # remove old file content
        f.seek(0)
        f.truncate()
        f.write(''.join(lines), )

def find_version_line(lines: list[str]) -> int:
    for i, line in enumerate(lines):
        if line.strip().startswith('const String VERSION'):
            return i
    return -1

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

    write_config_version(version)
    status_code = compile_sketch(BOARD, BUILD_DIR, SKETCH_PATH)

    if bool(status_code):
        print(f'Compilation failed with status code: {status_code}\nexiting...')
        sys.exit(status_code)

    if will_rename:
        print(f'Renaming build to {version}')
        rename_build(version)

if __name__ == '__main__':
    main()
