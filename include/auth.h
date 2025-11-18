#ifndef AUTH_H
#define AUTH_H

#include <time.h>  // time() 함수를 위해 추가
#include "models.h"

// 로그인 시도
int try_login(UserRecord *user, const char *password, UserTime *timeRec);

// 로그아웃 시도
int try_logout(UserRecord *user, int *outElapsedMinutes, UserTime *timeRec);

#endif


