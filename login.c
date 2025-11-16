// login.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "login.h"
#include "storage.h"
#include "auth.h"

void loginMenu(void) {
    char input_id[MAX_LEN];
    printf("아이디를 입력하세요 (로그인/로그아웃): ");
    scanf("%99s", input_id);

    UserRecord user;
    if (!loadUser(input_id, &user)) {
        printf("해당 ID를 찾을 수 없습니다.\n");
        return;
    }

    if (user.is_logged_in == 0) {
        // 로그인 UI: 비밀번호만 입력 받고 나머지는 로직에 위임
        char password_input[MAX_LEN];
        printf("비밀번호를 입력하세요: ");
        scanf("%99s", password_input);

        UserTime t;
        if (try_login(&user, password_input, &t)) {
            printf("[%s] 로그인 성공!\n", user.id);
            printf("현재 남은 시간: %d분\n", t.minutes);
        } else {
            printf("로그인 실패(비밀번호가 틀렸거나 이미 로그인된 ID입니다).\n");
        }
    } else {
        // 로그아웃 UI: 별도 정보 입력 없이 ID만 가지고 로직 호출
        int elapsed = 0;
        UserTime t;
        if (try_logout(&user, &elapsed, &t)) {
            if (elapsed > 0)
                printf("로그인 후 경과 시간: %d분\n", elapsed);
            printf("[%s] 로그아웃 완료! 남은 시간: %d분\n", user.id, t.minutes);
        } else {
            printf("로그아웃 실패(로그인 상태가 아니거나 데이터 오류).\n");
        }
    }
}
