// newUser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "newUser.h"
#include "storage.h"

void newUser(void) {
    char name[MAX_LEN], password[MAX_LEN];

    printf("새 회원 ID를 입력하세요: ");
    scanf("%99s", name);

    // 이미 존재하는지 확인
    UserRecord existing;
    if (loadUser(name, &existing)) {
        printf("이미 존재하는 회원 ID입니다.\n");
        return;
    }

    printf("비밀번호를 입력하세요: ");
    scanf("%99s", password);

    UserRecord user;
    memset(&user, 0, sizeof(user));
    strncpy(user.id, name, MAX_LEN - 1);
    strncpy(user.password, password, MAX_LEN - 1);
    user.is_logged_in = 0;

    UserTime timeRec;
    memset(&timeRec, 0, sizeof(timeRec));
    strncpy(timeRec.id, name, MAX_LEN - 1);
    timeRec.minutes = 0;

    if (!saveUser(&user) || !saveUserTime(&timeRec)) {
        printf("파일 저장 중 오류가 발생했습니다.\n");
        return;
    }

    printf("회원 등록이 완료되었습니다.\n");
}
