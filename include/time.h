#ifndef TIME_H
#define TIME_H

#include <time.h>  // time() 함수를 위해 추가
#include "models.h"

// 회원의 현재 남은 시간 계산 (로그인 중이면 경과 시간 반영)
int getRemainingTimeWithElapsed(const char *user_id, int *outRemaining);

// 게스트 경과 시간 계산 및 차감
void updateGuestRemainingTime(GuestInfo *guest);

#endif
