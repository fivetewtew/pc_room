// login.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 100

// 로그인 시간 기록 함수
void recordLoginTime(const char *username) {
    FILE *file = fopen("login_times.csv", "a");
    if (!file) {
        printf("로그인 시간 파일 열기 실패\n");
        return;
    }
    time_t now = time(NULL);
    fprintf(file, "%s,%ld\n", username, now);
    fclose(file);
}

// 로그인 시간 읽기 및 삭제
time_t getAndRemoveLoginTime(const char *username) {
    FILE *file = fopen("login_times.csv", "r");
    FILE *temp = fopen("login_times_temp.csv", "w");
    if (!file || !temp) {
        if (file) fclose(file);
        if (temp) fclose(temp);
        return 0;
    }

    char line[MAX_LEN], user[MAX_LEN];
    long login_time = 0;
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        long t;
        sscanf(line, "%[^,],%ld", user, &t);
        if (strcmp(user, username) == 0) {
            login_time = t;
            found = 1;
        } else {
            fprintf(temp, "%s", line);
        }
    }

    fclose(file);
    fclose(temp);

    remove("login_times.csv");
    rename("login_times_temp.csv", "login_times.csv");

    if (!found) return 0;
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

int main(void) {
    char input_id[MAX_LEN];
    char line[MAX_LEN];
    int found = 0;

    printf("아이디 입력 (로그인/로그아웃): ");
    scanf("%s", input_id);

    FILE *file = fopen("User.csv", "r");
    FILE *temp = fopen("User_temp.csv", "w");

    if (!file || !temp) {
        printf("파일 열기 실패\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        char user[MAX_LEN], pass[MAX_LEN], status[MAX_LEN];
        sscanf(line, "%[^,],%[^,],%s", user, pass, status);

        if (strcmp(user, input_id) == 0) {
            found = 1;

            if (strcmp(status, "0") == 0) {
                // 로그인
                char password_input[MAX_LEN];
                printf("비밀번호 입력: ");
                scanf("%s", password_input);

                if (strcmp(password_input, pass) == 0) {
                    fprintf(temp, "%s,%s,1\n", user, pass);
                    printf("[%s] 로그인 성공!\n", user);
                    recordLoginTime(user);  // 로그인 시각 기록

                    // 남은 시간 출력
                    int remain = getUserRemainingTime(user);
                    if(remain >= 0)
                        printf("? 남은 시간: %d분\n", remain);
                } else {
                    printf("비밀번호가 틀렸습니다.\n");
                    fprintf(temp, "%s,%s,%s\n", user, pass, status);
                }
            } else {
                // 로그아웃 - 경과 시간 차감
                time_t login_time = getAndRemoveLoginTime(user);
                time_t now = time(NULL);
                int elapsed_min = 0;

                if (login_time > 0) {
                    elapsed_min = (int)((now - login_time) / 60);
                    printf("로그인 후 경과 시간: %d분\n", elapsed_min);
                } else {
                    printf("로그인 시간이 기록되지 않았습니다.\n");
                }

                int remaining = getUserRemainingTime(user);
                if (remaining < 0) remaining = 0;

                int updated = remaining - elapsed_min;
                if (updated < 0) updated = 0;

                updateUserRemainingTime(user, updated);

                printf("[%s] 로그아웃 완료! 남은 시간: %d분\n", user, updated);
                fprintf(temp, "%s,%s,0\n", user, pass);
            }
        } else {
            fprintf(temp, "%s", line);
        }
    }

    fclose(file);
    fclose(temp);

    if (!found) {
        printf("해당 ID를 찾을 수 없습니다.\n");
        remove("User_temp.csv");
        return 1;
    }

    remove("User.csv");
    rename("User_temp.csv", "User.csv");

    return 0;
}

