#ifndef MODELS_H
#define MODELS_H

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

#endif


