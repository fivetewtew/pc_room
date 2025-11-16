// charge.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "charge.h"
#include "storage.h"

#define MAX_LEN 100

// User.csv에서 회원 존재 여부 확인
static int userExists(const char* username) {
    FILE *file = fopen("User.csv", "r");
    if (!file) return 0;

    char line[MAX_LEN], user[MAX_LEN];

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],", user);
        if (strcmp(user, username) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

// User.csv에서 로그인 상태인지 확인
static int isUserLoggedIn(const char* username) {
    FILE *file = fopen("User.csv", "r");
    if (!file) return 0;

    char line[MAX_LEN], user[MAX_LEN], pass[MAX_LEN], status[MAX_LEN];

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%[^,],%s", user, pass, status);
        if (strcmp(user, username) == 0) {
            fclose(file);
            return strcmp(status, "1") == 0;
        }
    }

    fclose(file);
    return 0;
}

// 로그인 시간을 읽는 함수 (로그인 중인 경우)
static time_t getLoginTime(const char *username) {
    FILE *file = fopen("login_times.csv", "r");
    if (!file) return 0;

    char line[MAX_LEN], user[MAX_LEN];
    long login_time = 0;

    while (fgets(line, sizeof(line), file)) {
        long t;
        sscanf(line, "%[^,],%ld", user, &t);
        if (strcmp(user, username) == 0) {
            login_time = t;
            break;
        }
    }

    fclose(file);
    return (time_t)login_time;
}

static int chargeTime(const char *username) {
    int choice, added_time = 0, price = 0, cash;

    printf("\n충전할 시간을 선택하세요:\n");
    printf("1. 1시간 (1000원)\n2. 2시간 (1800원)\n3. 3시간 (3500원)\n선택: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: added_time = 60; price = 1000; break;
        case 2: added_time = 120; price = 1800; break;
        case 3: added_time = 180; price = 3500; break;
        default:
            printf("잘못된 선택입니다.\n");
            return 0;
    }

    printf("투입할 금액을 입력하세요: ");
    scanf("%d", &cash);

    if (cash < price) {
        printf("금액이 부족합니다. 입력하신 %d원을 그대로 반환합니다.\n", cash);
        return 0;
    }

    printf("%d원을 결제했습니다. %d분이 충전됩니다.\n", price, added_time);
    printf("잔돈 %d원을 거슬러드립니다.\n", cash - price);

    // 사용자가 현재 로그인 중인지 확인
    if (isUserLoggedIn(username)) {
        // 로그인 후 경과 시간만큼 차감하고 충전
        time_t login_time = getLoginTime(username);
        time_t now = time(NULL);
        
        if (login_time > 0) {
            int elapsed_min = (int)((now - login_time) / 60);
            int current_remaining = 0;
            if (!getUserTime(username, &current_remaining)) current_remaining = 0;
            
            // 경과 시간만큼 차감 후 충전 시간 더하기
            int updated_time = (current_remaining - elapsed_min) + added_time;
            if (updated_time < 0) updated_time = 0;
            
            setUserTime(username, updated_time);
            printf("로그인 후 %d분 사용, %d분 충전되어 총 %d분 남았습니다.\n", 
                   elapsed_min, added_time, updated_time);
        } else {
            // 로그인 시간이 없으면 단순 충전
            int remain = 0;
            if (!getUserTime(username, &remain)) remain = 0;
            remain += added_time;
            setUserTime(username, remain);
            printf("충전 후 남은 시간: %d분\n", remain);
        }
    } else {
        // 로그인 상태가 아니어도 시간만 충전
        int remain = 0;
        if (!getUserTime(username, &remain)) remain = 0;
        remain += added_time;
        setUserTime(username, remain);
        printf("충전 후 남은 시간: %d분\n", remain);
    }
    
    return 1;
}

void chargeMenu(void) {
    char input_id[MAX_LEN];
    printf("충전할 회원 ID를 입력하세요: ");
    scanf("%s", input_id);

    // 회원 존재 여부 확인
    if (!userExists(input_id)) {
        printf("존재하지 않는 ID입니다.\n");
        return;
    }

    if (!chargeTime(input_id)) {
        printf("충전이 취소되었습니다.\n");
        return;
    }

    printf("충전이 완료되었습니다.\n");
}
