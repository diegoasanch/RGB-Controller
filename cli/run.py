#!/usr/bin/env python3
import sys
import subprocess
import os
import shutil
from typing import Optional

from compile import is_version_valid, compile_sketch, BOARD


def check_arduino_cli() -> bool:
    """Check if arduino-cli is installed and accessible"""
    try:
        subprocess.run(['arduino-cli', '--version'],
                       stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE)
        return True
    except FileNotFoundError:
        print("Error: arduino-cli not found. Please install it first:")
        print("https://arduino.github.io/arduino-cli/latest/installation/")
        return False


def upload_binary(version: str, board: str, port: str) -> int:
    """Upload the binary to the board"""
    binary_path = f'./build/bin/{version}.bin'
    if not os.path.exists(binary_path):
        print(f"Error: Binary file not found at {binary_path}")
        return 1

    upload_cmd = f'arduino-cli upload -i {binary_path} -b {board} -p {port}'
    print("\nUploading binary to board...")

    process = subprocess.run(upload_cmd, shell=True, text=True,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    if process.returncode != 0:
        print("Error during upload:")
        print(process.stderr)
        return process.returncode

    print("Upload successful!")
    return 0


def monitor_board(port: str, baudrate: int = 115200) -> None:
    """Monitor the board's serial output"""
    monitor_cmd = f'arduino-cli monitor -p {port} -c baudrate={baudrate}'
    print("\nAttaching to board serial monitor...")
    print("Press Ctrl+C to exit")

    try:
        subprocess.run(monitor_cmd, shell=True)
    except KeyboardInterrupt:
        print("\nDetached from serial monitor")


def main():
    if not check_arduino_cli():
        sys.exit(1)

    if len(sys.argv) != 2:
        print("Usage: python run.py <version>")
        print("Example: python run.py 1.0.0")
        sys.exit(1)

    version = sys.argv[1]
    port = '/dev/cu.usbserial-120'

    # Validate version
    if not is_version_valid(version):
        print(f"Error: Invalid version format: {version}")
        print("Version must follow semantic versioning (e.g., 1.0.0)")
        sys.exit(1)

    # Step 1: Compile
    print(f"\nCompiling version {version}...")
    compile_result = compile_sketch(BOARD, './build', './src/src.ino')

    # Check if binary was actually created
    binary_path = f'./build/src.ino.bin'
    if compile_result != 0 or not os.path.exists(binary_path):
        print("Compilation failed - no binary produced")
        sys.exit(1)

    # Move binary to versioned name
    os.makedirs('./build/bin', exist_ok=True)
    versioned_binary = f'./build/bin/{version}.bin'
    shutil.copy2(binary_path, versioned_binary)

    # Step 2: Upload
    if upload_binary(version, BOARD, port) != 0:
        print("Upload failed")
        sys.exit(1)

    # Step 3: Monitor
    monitor_board(port)


if __name__ == '__main__':
    main()
