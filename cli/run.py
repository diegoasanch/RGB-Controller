#!/usr/bin/env python3
import sys
import subprocess
import os
import shutil
import argparse
import socket
import requests
import threading
from typing import Optional
import time

from compile import is_version_valid, compile_sketch, BOARD
from server import app as update_server


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


def get_local_ip() -> str:
    """Get the local IP address of this machine"""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't need to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP


def start_update_server():
    """Start the update server in a separate thread"""
    thread = threading.Thread(target=update_server.run, kwargs={
        'host': '0.0.0.0',
        'port': 3301,
        'debug': False,
        'use_reloader': False
    })
    thread.daemon = True
    thread.start()
    return thread


def trigger_ota_update(controller_ip: str, server_ip: str, server_port: int = 3301) -> bool:
    """Trigger OTA update on the controller"""
    url = f'http://{controller_ip}/settings/version/update/{server_ip}/{server_port}/update'
    try:
        response = requests.post(url)
        return response.status_code == 200
    except requests.exceptions.RequestException as e:
        print(f"Error triggering OTA update: {e}")
        return False


def wait_for_update(controller_ip: str, timeout: int = 60) -> bool:
    """Wait for the controller to complete the update"""
    print("\nWaiting for update to complete...")
    print("(This may take up to a minute)")

    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            # Try to connect to the controller
            response = requests.get(f'http://{controller_ip}/', timeout=2)
            if response.status_code == 200:
                print("\nController is back online!")
                return True
        except requests.exceptions.RequestException:
            # If we can't connect, the controller might be updating
            print(".", end="", flush=True)
            time.sleep(2)

    print("\nTimeout waiting for controller to come back online")
    return False


def main():
    parser = argparse.ArgumentParser(
        description='Build and upload firmware to ESP8266')
    parser.add_argument('version', help='Version number (semver)')
    parser.add_argument('--ota', help='Controller IP for OTA update')
    parser.add_argument('--timeout', type=int, default=60,
                        help='Timeout in seconds for OTA update (default: 60)')
    args = parser.parse_args()

    version = args.version
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

    if args.ota:
        print("\nStarting OTA update...")
        # Start update server
        print("Starting update server...")
        server_thread = start_update_server()

        # Give the server a moment to start
        time.sleep(1)

        # Get local IP for the update server
        local_ip = get_local_ip()
        print(f"Update server running at {local_ip}:3301")

        # Trigger update on controller
        print(f"Triggering update on controller at {args.ota}...")
        if not trigger_ota_update(args.ota, local_ip):
            print("Failed to trigger OTA update")
            sys.exit(1)

        print("\nOTA update triggered successfully!")
        print("Waiting for the controller to download and install the update...")

        # Wait for the update to complete
        if wait_for_update(args.ota, args.timeout):
            print("OTA update completed successfully!")
        else:
            print("Warning: Could not confirm if update completed successfully")
            print("Please check the controller manually")

        # Keep the server running for a few more seconds to ensure any remaining requests are handled
        time.sleep(5)
        sys.exit(0)
    else:
        if not check_arduino_cli():
            sys.exit(1)

        # Step 2: Upload via USB
        if upload_binary(version, BOARD, port) != 0:
            print("Upload failed")
            sys.exit(1)

        # Step 3: Monitor
        monitor_board(port)


if __name__ == '__main__':
    main()
