import os
import re

from constants import version_regex
from flask import Flask, send_from_directory, request, abort

app = Flask(__name__, static_folder='../build/bin')


@app.route('/latest_version', methods=['GET'])
def latest_version() -> str:
    '''
    Get the latest version from the build directory
    '''
    latest_version = get_latest_version(app.static_folder)
    if latest_version:
        return latest_version
    else:
        abort(500, "Internal server error. No versions found.")


@app.route('/update', methods=['GET'])
def update_version():
    'Get the latest build from the build directory'
    latest_version = get_latest_version(app.static_folder)
    if latest_version:
        try:
            return send_from_directory(app.static_folder, f'{latest_version}.bin', as_attachment=True)
        except FileNotFoundError:
            abort(404, "File not found.")
    else:
        abort(400, "Bad request. Please provide a version parameter.")


def get_latest_version(directory: str) -> (str | None):
    '''
    Latest version string from the build directory
    '''
    versions = [
        f.rstrip('.bin') for f in os.listdir(directory)
    ]

    if len(versions):
        latest_version = max(versions)
        return latest_version
    else:
        return None


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=3301)
