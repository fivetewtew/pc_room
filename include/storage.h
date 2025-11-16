#ifndef STORAGE_H
#define STORAGE_H

#include <time.h>
#include "models.h"

// User.csv: id,password,status(0/1)
// 회원 기본 정보 로드/저장
int loadUser(const char *id, UserRecord *outUser);       // 1=성공,0=없음
int saveUser(const UserRecord *user);                    // 1=성공,0=실패

// User_time.csv: id,minutes
// 회원 남은 시간 로드/저장
int loadUserTime(const char *id, UserTime *outTime);     // 1=성공,0=없음
int saveUserTime(const UserTime *timeRec);               // 1=성공,0=실패

// login_times.csv: id,time_t
// 로그인 시각 기록/제거(pop)
int addLoginTime(const char *id, time_t t);              // 1=성공,0=실패
int popLoginTime(const char *id, time_t *out_t);         // 1=성공,0=없음

// Guest_time.csv: id,remain_time,last_time
// 손님(게스트) 남은 시간 로드/저장
int loadGuestInfo(const char *id, GuestInfo *outGuest);  // 1=성공,0=없음
int saveGuestInfo(const GuestInfo *guest);               // 1=성공,0=실패

#endif


