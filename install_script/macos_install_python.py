# This project is licensed under the 'BSD 2-clause license'.

# Copyright (c) 2017-2019, Joe Rickerby and contributors. All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.

# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


from pathlib import Path
import os
import ssl
import urllib.request
import subprocess
import certifi

# python_configurations = [
#   { identifier: "cp36-macosx_x86_64", version: "3.6", url: "https://www.python.org/ftp/python/3.6.8/python-3.6.8-macosx10.9.pkg" },
#   { identifier: "cp37-macosx_x86_64", version: "3.7", url: "https://www.python.org/ftp/python/3.7.9/python-3.7.9-macosx10.9.pkg" },
#   { identifier: "cp38-macosx_x86_64", version: "3.8", url: "https://www.python.org/ftp/python/3.8.10/python-3.8.10-macosx10.9.pkg" },
#   { identifier: "cp38-macosx_arm64", version: "3.8", url: "https://www.python.org/ftp/python/3.8.10/python-3.8.10-macosx10.9.pkg" },
#   { identifier: "cp38-macosx_universal2", version: "3.8", url: "https://www.python.org/ftp/python/3.8.10/python-3.8.10-macosx10.9.pkg" },
#   { identifier: "cp39-macosx_x86_64", version: "3.9", url: "https://www.python.org/ftp/python/3.9.7/python-3.9.7-macos11.pkg" },
#   { identifier: "cp39-macosx_arm64", version: "3.9", url: "https://www.python.org/ftp/python/3.9.7/python-3.9.7-macos11.pkg" },
#   { identifier: "cp39-macosx_universal2", version: "3.9", url: "https://www.python.org/ftp/python/3.9.7/python-3.9.7-macos11.pkg" },
#   { identifier: "cp310-macosx_x86_64", version: "3.10", url: "https://www.python.org/ftp/python/3.10.0/python-3.10.0-macos11.pkg" },
#   { identifier: "cp310-macosx_arm64", version: "3.10", url: "https://www.python.org/ftp/python/3.10.0/python-3.10.0-macos11.pkg" },
#   { identifier: "cp310-macosx_universal2", version: "3.10", url: "https://www.python.org/ftp/python/3.10.0/python-3.10.0-macos11.pkg" },
#   { identifier: "pp37-macosx_x86_64", version: "3.7", url: "https://downloads.python.org/pypy/pypy3.7-v7.3.5-osx64.tar.bz2" },
# ]

install_certifi_script = Path(__file__).parent / "install_certifi.py"

def download(url: str, dest: Path) -> None:
    print(f"+ Download {url} to {dest}")
    dest_dir = dest.parent
    if not dest_dir.exists():
        dest_dir.mkdir(parents=True)

    # we've had issues when relying on the host OS' CA certificates on Windows,
    # so we use certifi (this sounds odd but requests also does this by default)
    cafile = os.environ.get("SSL_CERT_FILE", certifi.where())
    context = ssl.create_default_context(cafile=cafile)
    repeat_num = 3
    for i in range(repeat_num):
        try:
            response = urllib.request.urlopen(url, context=context)
        except Exception:
            if i == repeat_num - 1:
                raise
            sleep(3)
            continue
        break

    try:
        dest.write_bytes(response.read())
    finally:
        response.close()


def install_cpython(version: str, url: str) -> Path:
    installed_system_packages = subprocess.run(
        ["pkgutil", "--pkgs"], universal_newlines=True, check=True, stdout=subprocess.PIPE
    ).stdout.splitlines()

    # if this version of python isn't installed, get it from python.org and install
    python_package_identifier = f"org.python.Python.PythonFramework-{version}"
    python_executable = "python3"
    installation_bin_path = Path(f"/Library/Frameworks/Python.framework/Versions/{version}/bin")

    if python_package_identifier not in installed_system_packages:
        # download the pkg
        download(url, Path("/tmp/Python.pkg"))
        # install
        call(["sudo", "installer", "-pkg", "/tmp/Python.pkg", "-target", "/"])
        call(["sudo", str(installation_bin_path / python_executable), str(install_certifi_script)])

    pip_executable = "pip3"
    make_symlinks(installation_bin_path, python_executable, pip_executable)

    return installation_bin_path


def main():

    version = "3.9"
    url = "https://www.python.org/ftp/python/3.9.7/python-3.9.7-macos11.pkg"

    install_cpython(version, url)


if __name__ == '__main__':
    main()
