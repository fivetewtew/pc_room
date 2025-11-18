// time.c
// 시간 계산 로직을 담당합니다. 회원/게스트의 경과 시간 계산 및 차감을 처리합니다.
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "time.h"
#include "storage.h"

// time() 함수 명시적 선언 (경고 제거)
time_t time(time_t *timer);

// 회원의 현재 남은 시간 계산 (로그인 중이면 경과 시간 반영)
int getRemainingTimeWithElapsed(const char *user_id, int *outRemaining) {
    if (!user_id || !outRemaining) return 0;

    UserRecord user;
    if (!loadUser(user_id, &user)) {
        return 0;  // 회원 정보 없음
    }

    UserTime t;
    int has_time = loadUserTime(user_id, &t);
    if (!has_time) {
        *outRemaining = 0;
        return 1;  // 시간 정보 없으면 0분
    }

    int remaining = t.minutes;
    
    // 로그인 중이면 경과 시간 차감
    if (user.is_logged_in) {
        time_t login_time;
        if (getLoginTime(user_id, &login_time)) {
            time_t now = time(NULL);
            int elapsed = (int)((now - login_time) / 60);
            if (elapsed > 0) {
                remaining -= elapsed;
                if (remaining < 0) remaining = 0;
            }
        }
    }

    *outRemaining = remaining;
    return 1;
}

// 게스트 경과 시간 계산 및 차감
void updateGuestRemainingTime(GuestInfo *guest) {
    if (!guest) return;

    time_t now = time(NULL);
    int elapsed_min = (int)((now - guest->last_time) / 60);
    
    if (elapsed_min > 0) {
        guest->remain_time -= elapsed_min;
        if (guest->remain_time < 0) guest->remain_time = 0;
        guest->last_time = now;
    }
}
