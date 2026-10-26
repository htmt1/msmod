#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
python3 -m venv .tools
PIP_CACHE_DIR="$PWD/build/pip-cache" .tools/bin/python -m pip install --disable-pip-version-check -r requirements-dev.txt
ZIG="$PWD/.tools/lib/python$(.tools/bin/python -c 'import sys;print(f"{sys.version_info.major}.{sys.version_info.minor}")')/site-packages/ziglang/zig" bash tools/build.sh
bash tools/test.sh
.tools/bin/python tools/validate_artifacts.py

.tools/bin/python tests/proxy_abi_tests.py
