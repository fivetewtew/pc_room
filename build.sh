#!/usr/bin/env bash
set -euo pipefail

# 스크립트 위치(프로젝트 루트: pc_room)로 이동
cd "$(dirname "$0")"

CFLAGS="-Wall -Wextra -O2 -Iinclude"

UI_SRC="src/ui/pc_room.c src/ui/login.c src/ui/charge.c src/ui/Guest.c src/ui/newUser.c"
CORE_SRC="src/core/auth.c"
STORAGE_SRC="src/storage/storage.c"
CONFIG_SRC="src/config/config.c"

echo "[1/1] gcc 컴파일/링크 중..."
gcc ${CFLAGS} ${UI_SRC} ${CORE_SRC} ${STORAGE_SRC} ${CONFIG_SRC} -o pc_room
echo "빌드 완료 → ./pc_room"


