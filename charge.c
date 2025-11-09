// charge.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 100

// User.csv에서 사용자 존재 여부 확인
int userExists(const char* username) {
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

// User.csv에서 로그인 상태 확인
int isUserLoggedIn(const char* username) {
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

// 로그인 시간 읽기 함수 (삭제하지 않음)
time_t getLoginTime(const char *username) {
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

// User_time.csv에서 남은 시간 읽기
int getUserRemainingTime(const char* username) {
    FILE *file = fopen("User_time.csv", "r");
    if (!file) return -1;

    char line[MAX_LEN], name[MAX_LEN];
    int time_val = -1;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%d", name, &time_val);
        if (strcmp(name, username) == 0) {
            fclose(file);
            return time_val;
        }
    }

    fclose(file);
    return -1;
}

// User_time.csv에서 남은 시간 업데이트 함수
void updateUserRemainingTime(const char* username, int new_time) {
    FILE *file = fopen("User_time.csv", "r");
    FILE *temp = fopen("User_time_temp.csv", "w");
    if (!file || !temp) return;

    char line[MAX_LEN], name[MAX_LEN];
    int time_val;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%d", name, &time_val);
        if (strcmp(name, username) == 0) {
            fprintf(temp, "%s,%d\n", name, new_time > 0 ? new_time : 0);
        } else {
            fprintf(temp, "%s", line);
        }
    }

    fclose(file);
    fclose(temp);

    remove("User_time.csv");
    rename("User_time_temp.csv", "User_time.csv");
}

int chargeTime(const char *username) {
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

    printf("돈을 넣어주세요: ");
    scanf("%d", &cash);

    if (cash < price) {
        printf("돈이 부족합니다. 현재 넣은 돈 %d원 반환합니다.\n", cash);
        return 0;
    }

    printf("%d원 결제 완료! %d분 충전됩니다.\n", price, added_time);
    printf("잔돈 %d원 반환합니다.\n", cash - price);

    // 사용자가 현재 로그인 중인지 확인
    if (isUserLoggedIn(username)) {
        // 로그인 중이면 사용한 시간만큼 차감 후 충전
        time_t login_time = getLoginTime(username);
        time_t now = time(NULL);
        
        if (login_time > 0) {
            int elapsed_min = (int)((now - login_time) / 60);
            int current_remaining = getUserRemainingTime(username);
            
            if (current_remaining < 0) current_remaining = 0;
            
            // 사용한 시간 차감 후 충전
            int updated_time = (current_remaining - elapsed_min) + added_time;
            if (updated_time < 0) updated_time = 0;
            
            updateUserRemainingTime(username, updated_time);
            printf("로그인 중 사용한 %d분 차감 후 %d분 충전되어 총 %d분 남았습니다.\n", 
                   elapsed_min, added_time, updated_time);
        } else {
            // 로그인 시간 기록이 없으면 그냥 충전
            int remain = getUserRemainingTime(username);
            if (remain < 0) remain = 0;
            remain += added_time;
            updateUserRemainingTime(username, remain);
            printf("? 충전 후 남은 시간: %d분\n", remain);
        }
    } else {
        // 로그인 중이 아니면 그냥 충전
        int remain = getUserRemainingTime(username);
        if (remain < 0) remain = 0;
        remain += added_time;
        updateUserRemainingTime(username, remain);
        printf("? 충전 후 남은 시간: %d분\n", remain);
    }
    
    return 1;
}

int main(void) {
    char input_id[MAX_LEN];
    printf("아이디 입력: ");
    scanf("%s", input_id);

    // 사용자 존재 여부 확인
    if (!userExists(input_id)) {
        printf("잘못된 아이디입니다.\n");
        return 0;
    }

    if (!chargeTime(input_id)) {
        printf("메인 메뉴로 돌아갑니다.\n");
        return 0;
    }

    printf("충전 완료!\n");
    return 1;
}
