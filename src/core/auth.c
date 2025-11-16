// auth.c
// 로그인/로그아웃 로직(순수 비즈니스)만 담당합니다. UI나 printf는 여기서 하지 않습니다.
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "auth.h"
#include "storage.h"

int try_login(UserRecord *user, const char *password, UserTime *timeRec) {
    if (!user || !password) return 0;

    // 이미 로그인 상태면 실패로 간주
    if (user->is_logged_in != 0) {
        return 0;
    }

    // 비밀번호 검증
    if (strcmp(password, user->password) != 0) {
        return 0;
    }

    // 상태 변경 및 저장
    user->is_logged_in = 1;
    if (!saveUser(user)) {
        return 0;
    }

    // 로그인 시간 기록
    addLoginTime(user->id, time(NULL));

    // 남은 시간 정보 반환
    if (timeRec) {
        UserTime t;
        if (loadUserTime(user->id, &t)) {
            *timeRec = t;
        } else {
            memset(&t, 0, sizeof(t));
            strncpy(t.id, user->id, MAX_LEN - 1);
            t.minutes = 0;
            *timeRec = t;
        }
    }

    return 1;
}

int try_logout(UserRecord *user, int *outElapsedMinutes, UserTime *timeRec) {
    if (!user) return 0;

    // 로그인 상태가 아니면 실패
    if (user->is_logged_in == 0) {
        return 0;
    }

    time_t login_time;
    int has_login = popLoginTime(user->id, &login_time);
    time_t now = time(NULL);
    int elapsed_min = 0;

    if (has_login && login_time > 0) {
        elapsed_min = (int)((now - login_time) / 60);
    }

    if (outElapsedMinutes) {
        *outElapsedMinutes = elapsed_min;
    }

    UserTime t;
    int has_time = loadUserTime(user->id, &t);
    if (!has_time) {
        memset(&t, 0, sizeof(t));
        strncpy(t.id, user->id, MAX_LEN - 1);
    }

    int updated = t.minutes - elapsed_min;
    if (updated < 0) updated = 0;
    t.minutes = updated;
    saveUserTime(&t);

    user->is_logged_in = 0;
    saveUser(user);

    if (timeRec) {
        *timeRec = t;
    }

    return 1;
}


