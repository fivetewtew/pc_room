#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

# UTF-8 플래그 추가 (build.sh와 동일하게)
CFLAGS="-Wall -Wextra -O2 -Iinclude -finput-charset=UTF-8 -fexec-charset=UTF-8"

# Test source files
TEST_SRC="src/test/test_ui.c"
CORE_SRC="src/core/auth.c src/core/time.c src/core/billing.c"
STORAGE_SRC="src/storage/storage.c"
CONFIG_SRC="src/config/config.c"

echo "[1/1] Compiling test code..."
gcc ${CFLAGS} ${TEST_SRC} ${CORE_SRC} ${STORAGE_SRC} ${CONFIG_SRC} -o test_ui
echo "Build complete → ./test_ui"
