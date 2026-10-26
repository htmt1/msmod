#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build
for test in ledger_tests decimal_format_tests startup_policy_tests wallet_ui_model_tests; do
  g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -g "tests/$test.cpp" -o "build/$test"
  "build/$test"
done
