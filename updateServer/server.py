from flask import Flask, send_from_directory, request, abort

import os
import re
from packaging import version

def get_latest_version(directory):
    semver_files = [f for f in os.listdir(directory)
                    if re.fullmatch(r'v\d+\.\d+\.\d+\.bin', f)]

    versions = [version.parse(f[1:-4]) for f in semver_files]

    if versions:
        latest_version = max(versions)
        return f"v{latest_version}"
    else:
        return None


app = Flask(__name__, static_folder='../build/bin')

@app.route('/latest_version', methods=['GET'])
def latest_version():
    latest_version = get_latest_version(app.static_folder)
    if latest_version:
        return latest_version
    else:
        abort(500, "Internal server error. No versions found.")


@app.route('/update', methods=['GET'])
def update_version():
    latest_version = get_latest_version(app.static_folder)
    if latest_version:
        try:
            return send_from_directory(app.static_folder, f'{latest_version}.bin', as_attachment=True)
        except FileNotFoundError:
            abort(404, "File not found.")
    else:
        abort(400, "Bad request. Please provide a version parameter.")

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=3301)
