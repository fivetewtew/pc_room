// auth.c
// 로그인/로그아웃 로직(순수 비즈니스)만 담당합니다. UI나 printf는 여기서 하지 않습니다.
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "auth.h"
#include "storage.h"

// time() 함수 명시적 선언 (경고 제거)
time_t time(time_t *timer);

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
            snprintf(t.id, sizeof(t.id), "%s", user->id);
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
    // 로그인 시간 조회 및 삭제 (popLoginTime은 조회 후 삭제함)
    time_t login_time;
    int has_login = popLoginTime(user->id, &login_time);
    time_t now = time(NULL);
    int elapsed_min = 0;
    // 로그인 시간이 있으면 경과 시간 계산 (분 단위)
    if (has_login && login_time > 0) {
        elapsed_min = (int)((now - login_time) / 60);
    }
    // 경과 시간을 외부로 반환
    if (outElapsedMinutes) {
        *outElapsedMinutes = elapsed_min;
    }
    // 현재 남은 시간 조회
    UserTime t;
    int has_time = loadUserTime(user->id, &t);
    if (!has_time) {
        // 시간 정보가 없으면 초기화
        memset(&t, 0, sizeof(t));
        snprintf(t.id, sizeof(t.id), "%s", user->id);
    }
    // 경과 시간만큼 차감 (음수 방지)
    int updated = t.minutes - elapsed_min;
    if (updated < 0) updated = 0;
    t.minutes = updated;
    saveUserTime(&t);  // 업데이트된 시간 저장
    // 로그인 상태 해제 및 저장
    user->is_logged_in = 0;
    saveUser(user);
    // 업데이트된 시간 정보 반환
    if (timeRec) {
        *timeRec = t;
    }
    return 1;  // 로그아웃 성공
}


