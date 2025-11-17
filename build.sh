#!/usr/bin/env bash
set -euo pipefail  # 에러 발생 시 스크립트 중단, 미정의 변수 사용 시 에러, 파이프라인 실패 시 중단

# 스크립트 위치(프로젝트 루트: pc_room)로 이동
cd "$(dirname "$0")"

# 표준 빌드 플래그 (Git Bash/mintty는 기본 UTF-8이므로 별도 문자셋 플래그 불필요)
# -Wall: 모든 경고 표시, -Wextra: 추가 경고, -O2: 최적화 레벨 2, -Iinclude: 헤더 파일 경로
CFLAGS="-Wall -Wextra -O2 -Iinclude"

# UI 관련 소스 파일들
UI_SRC="src/ui/pc_room.c src/ui/login.c src/ui/charge.c src/ui/Guest.c src/ui/newUser.c"
# 핵심 로직 소스 파일 (인증 관련)
CORE_SRC="src/core/auth.c"
# 저장소 관련 소스 파일 (CSV 파일 입출력)
STORAGE_SRC="src/storage/storage.c"
# 설정 관련 소스 파일 (상품 정보 등)
CONFIG_SRC="src/config/config.c"

echo "[1/1] gcc 컴파일/링크 중..."
# 모든 소스 파일을 컴파일하고 링크하여 실행 파일 생성
gcc ${CFLAGS} ${UI_SRC} ${CORE_SRC} ${STORAGE_SRC} ${CONFIG_SRC} -o pc_room
echo "빌드 완료 → ./pc_room"


