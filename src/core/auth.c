// auth.c
// 로그인/로그아웃 로직(순수 비즈니스)만 담당합니다. UI나 printf는 여기서 하지 않습니다.
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "auth.h"
#include "storage.h"

// time() 함수 명시적 선언 (경고 제거)
time_t time(time_t *timer);

/**
 * 회원 로그인 시도
 * 
 * @param user 회원 정보 (입력/출력)
 * @param password 입력받은 비밀번호
 * @param timeRec 남은 시간 정보를 반환할 포인터 (NULL 가능)
 * @return 1: 로그인 성공, 0: 로그인 실패
 * 
 * 동작 과정:
 * 1. 이미 로그인 상태인지 확인
 * 2. 비밀번호 검증
 * 3. 로그인 상태로 변경 및 저장
 * 4. 로그인 시간 기록 (Epoch Time)
 * 5. 남은 시간 정보 반환 (옵션)
 */
int try_login(UserRecord *user, const char *password, UserTime *timeRec) {
    if (!user || !password) return 0;

    // 이미 로그인 상태면 실패로 간주 (중복 로그인 방지)
    if (user->is_logged_in != 0) {
        return 0;
    }

    // 비밀번호 검증
    if (strcmp(password, user->password) != 0) {
        return 0;
    }

    // 로그인 상태로 변경 및 저장
    user->is_logged_in = 1;
    if (!saveUser(user)) {
        return 0;  // 저장 실패 시 로그인 실패
    }

    // 로그인 시간 기록 (Epoch Time 사용)
    addLoginTime(user->id, time(NULL));

    // 남은 시간 정보 반환 (Output Parameter Pattern)
    if (timeRec) {
        UserTime t;
        if (loadUserTime(user->id, &t)) {
            *timeRec = t;  // 시간 정보가 있으면 반환
        } else {
            // 시간 정보가 없으면 0분으로 초기화하여 반환
            memset(&t, 0, sizeof(t));
            snprintf(t.id, sizeof(t.id), "%s", user->id);
            t.minutes = 0;
            *timeRec = t;
        }
    }

    return 1;  // 로그인 성공
}

/**
 * 회원 로그아웃 처리
 * 
 * @param user 회원 정보 (입력/출력)
 * @param outElapsedMinutes 로그인 후 경과 시간(분)을 반환할 포인터 (NULL 가능)
 * @param timeRec 업데이트된 남은 시간 정보를 반환할 포인터 (NULL 가능)
 * @return 1: 로그아웃 성공, 0: 로그아웃 실패
 * 
 * 동작 과정:
 * 1. 로그인 상태 확인
 * 2. 로그인 시간 조회 및 삭제 (popLoginTime)
 * 3. 경과 시간 계산 (Epoch Time 차이를 분 단위로 변환)
 * 4. 남은 시간에서 경과 시간 차감
 * 5. 로그인 상태 해제 및 저장
 * 6. 경과 시간 및 업데이트된 남은 시간 반환
 */
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
    // Epoch Time 차이를 60으로 나누어 분 단위로 변환
    if (has_login && login_time > 0) {
        elapsed_min = (int)((now - login_time) / 60);
    }
    
    // 경과 시간을 외부로 반환 (Output Parameter Pattern)
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
    if (updated < 0) updated = 0;  // 음수가 되지 않도록 보정
    t.minutes = updated;
    saveUserTime(&t);  // 업데이트된 시간 저장
    
    // 로그인 상태 해제 및 저장
    user->is_logged_in = 0;
    saveUser(user);
    
    // 업데이트된 시간 정보 반환 (Output Parameter Pattern)
    if (timeRec) {
        *timeRec = t;
    }
    
    return 1;  // 로그아웃 성공
}


