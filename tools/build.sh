#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${ZIG:=zig}"
export ZIG_GLOBAL_CACHE_DIR="${ZIG_GLOBAL_CACHE_DIR:-$PWD/build/zig-global-cache}"
export ZIG_LOCAL_CACHE_DIR="${ZIG_LOCAL_CACHE_DIR:-$PWD/build/zig-cache}"
mkdir -p build dist
for source in buffer hook trampoline hde/hde64; do
  "$ZIG" cc -target x86_64-windows-gnu -O2 -DUNICODE -D_UNICODE -Ithird_party/minhook/include -c "third_party/minhook/src/$source.c" -o "build/$(basename "$source").o"
done
"$ZIG" c++ -target x86_64-windows-gnu -std=c++17 -O2 -DUNICODE -D_UNICODE -Wall -Wextra -shared src/plugin.cpp build/buffer.o build/hook.o build/trampoline.o build/hde64.o -o dist/MoonWallet.dll -lbcrypt
"$ZIG" c++ -target x86_64-windows-gnu -std=c++17 -O2 -DUNICODE -D_UNICODE -Wall -Wextra -static -municode -Wl,--subsystem,windows src/launcher.cpp -o dist/MoonWallet.exe -luser32 -lbcrypt
"$ZIG" c++ -target x86_64-windows-gnu -std=c++17 -O2 -DUNICODE -D_UNICODE -Wall -Wextra -shared src/version_proxy.cpp src/version_exports.S src/version_exports.def -o dist/version.dll -lkernel32
