#ifndef AUTH_H
#define AUTH_H

#include "models.h"

// 로그인 시도
// - 비밀번호가 맞고 아직 미로그인 상태면 user를 로그인 상태로 만들고 남은 시간을 timeRec로 반환
// - 성공 시 1, 실패 시 0 반환
int try_login(UserRecord *user, const char *password, UserTime *timeRec);

// 로그아웃 시도
// - 로그인 상태면 경과 분(로그인 시각 기준)을 계산해 남은 시간을 차감하여 저장
// - outElapsedMinutes: 경과 시간(분), timeRec: 갱신된 남은 시간
// - 성공 시 1, 실패 시 0 반환
int try_logout(UserRecord *user, int *outElapsedMinutes, UserTime *timeRec);

#endif


