// newUser.c
// 회원 등록 UI. 실제 저장소 접근은 storage 모듈을 사용합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "newUser.h"
#include "storage.h"

void newUser(void) {
    // setup_console_utf8() 제거 - main()에서 이미 설정됨
    
    char name[MAX_LEN], password[MAX_LEN];

    printf("새 회원 ID 입력: ");
    scanf("%99s", name);

    // ID 중복 확인
    UserRecord existing;
    if (loadUser(name, &existing)) {
        printf("이미 존재하는 ID입니다.\n");
        return;
    }

    printf("비밀번호 입력: ");
    scanf("%99s", password);

    // 새 회원/시간 초기화
    UserRecord user;
    memset(&user, 0, sizeof(user));
    snprintf(user.id, sizeof(user.id), "%s", name);
    snprintf(user.password, sizeof(user.password), "%s", password);
    user.is_logged_in = 0;

    UserTime timeRec;
    memset(&timeRec, 0, sizeof(timeRec));
    snprintf(timeRec.id, sizeof(timeRec.id), "%s", name);
    timeRec.minutes = 0;

    // 저장
    if (!saveUser(&user) || !saveUserTime(&timeRec)) {
        printf("데이터 파일 저장에 실패했습니다.\n");
        return;
    }

    printf("회원 등록이 완료되었습니다.\n");
}

