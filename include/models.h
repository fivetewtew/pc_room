#ifndef MODELS_H
#define MODELS_H

#include <time.h>  // time_t 타입을 위해 추가

// time_t가 정의되지 않은 경우를 대비한 fallback
#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
typedef long time_t;
#endif

#define MAX_LEN 100

// 회원 기본 정보
typedef struct {
    char id[MAX_LEN];
    char password[MAX_LEN];
    int  is_logged_in;   // 0 또는 1
} UserRecord;

// 회원 이용 시간 정보
typedef struct {
    char id[MAX_LEN];
    int  minutes;        // 남은 시간(분)
} UserTime;

// 손님(게스트) 이용 시간 정보
typedef struct {
    char id[MAX_LEN];
    int  remain_time;    // 남은 이용 시간(분)
    time_t last_time;    // 마지막 기준 시간 (epoch time)
} GuestInfo;

#endif



