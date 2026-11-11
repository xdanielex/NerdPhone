#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
cxx="${CXX:-g++}"
"$cxx" -std=c++17 -O2 -pthread tests/core_test.cpp -o "$tmp/core"
"$cxx" -std=c++17 -O2 tests/midstate_test.cpp -o "$tmp/midstate"
"$tmp/midstate"
"$tmp/core"
"$cxx" -std=c++17 -O2 -pthread tests/cpu_test.cpp -o "$tmp/cpu"
"$tmp/cpu"
NERDPHONE_TEST_EXSIZE=4 python3 tests/mock_pool.py
NERDPHONE_TEST_THREADS=2 NERDPHONE_TEST_EXSIZE=8 python3 tests/mock_pool.py
python3 tests/negotiation_test.py
