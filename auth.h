#ifndef AUTH_H
#define AUTH_H

#include "models.h"

// 로그인 시도: 비밀번호가 맞고 아직 미로그인 상태면
// user / timeRec를 갱신하고 1 반환, 아니면 0 반환
int try_login(UserRecord *user, const char *password, UserTime *timeRec);

// 로그아웃 시도: 로그인 상태면 경과 분 / 갱신된 시간 정보를 채우고 1 반환, 아니면 0 반환
int try_logout(UserRecord *user, int *outElapsedMinutes, UserTime *timeRec);

#endif


