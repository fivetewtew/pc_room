// time.c
// 시간 계산 로직을 담당합니다. 회원/게스트의 경과 시간 계산 및 차감을 처리합니다.
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "time.h"
#include "storage.h"

// time() 함수 명시적 선언 (경고 제거)
time_t time(time_t *timer);

/**
 * 회원의 현재 남은 시간 계산 (로그인 중이면 실시간 경과 시간 반영)
 * 
 * @param user_id 회원 ID
 * @param outRemaining 계산된 남은 시간(분)을 반환할 포인터
 * @return 1: 성공, 0: 실패 (회원 정보 없음)
 * 
 * 동작 과정:
 * 1. 회원 정보 로드
 * 2. 저장된 남은 시간 조회
 * 3. 로그인 중이면 Epoch Time을 사용하여 경과 시간 계산
 * 4. 경과 시간만큼 차감하여 실시간 남은 시간 반환
 * 
 * 핵심: 저장된 값이 아닌 실시간으로 계산된 정확한 남은 시간을 반환
 */
int getRemainingTimeWithElapsed(const char *user_id, int *outRemaining) {
    if (!user_id || !outRemaining) return 0;

    // 회원 정보 로드
    UserRecord user;
    if (!loadUser(user_id, &user)) {
        return 0;  // 회원 정보 없음
    }

    // 저장된 남은 시간 조회
    UserTime t;
    int has_time = loadUserTime(user_id, &t);
    if (!has_time) {
        *outRemaining = 0;
        return 1;  // 시간 정보 없으면 0분
    }

    int remaining = t.minutes;
    
    // 로그인 중이면 실시간 경과 시간 차감
    if (user.is_logged_in) {
        time_t login_time;
        if (getLoginTime(user_id, &login_time)) {
            time_t now = time(NULL);  // 현재 시간 (Epoch Time)
            // Epoch Time 차이를 분 단위로 변환
            int elapsed = (int)((now - login_time) / 60);
            if (elapsed > 0) {
                remaining -= elapsed;  // 경과 시간 차감
                if (remaining < 0) remaining = 0;  // 음수 방지
            }
        }
    }

    *outRemaining = remaining;  // 계산된 남은 시간 반환
    return 1;
}

/**
 * 게스트의 경과 시간 계산 및 남은 시간 차감
 * 
 * @param guest 게스트 정보 (입력/출력)
 * 
 * 동작 과정:
 * 1. last_time을 기준으로 현재까지 경과 시간 계산 (Epoch Time 사용)
 * 2. 경과 시간만큼 남은 시간 차감
 * 3. last_time을 현재 시간으로 업데이트 (다음 계산을 위한 기준점 설정)
 * 
 * 핵심: last_time을 업데이트하여 다음 호출 시 정확한 경과 시간 계산 보장
 */
void updateGuestRemainingTime(GuestInfo *guest) {
    if (!guest) return;

    time_t now = time(NULL);  // 현재 시간 (Epoch Time)
    // last_time을 기준으로 경과 시간 계산 (분 단위)
    int elapsed_min = (int)((now - guest->last_time) / 60);
    
    if (elapsed_min > 0) {
        // 경과 시간만큼 남은 시간 차감
        guest->remain_time -= elapsed_min;
        if (guest->remain_time < 0) guest->remain_time = 0;  // 음수 방지
        
        // 기준 시간을 현재 시간으로 업데이트
        // 다음 호출 시 이 시점부터 경과 시간을 계산하게 됨
        guest->last_time = now;
    }
}
