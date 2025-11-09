// newUser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

int main(void) {
    char name[MAX_LEN], password[MAX_LEN], line[MAX_LEN];

    printf("사용자 이름 입력: ");
    scanf("%s", name);

    FILE *file = fopen("User.csv", "r");
    if (file) {
        while (fgets(line, MAX_LEN, file)) {
            char *token = strtok(line, ",");
            if (token && strcmp(token, name) == 0) {
                printf("이미 존재하는 사용자 이름입니다.\n");
                fclose(file);
                return 1;
            }
        }
        fclose(file);
    }

    printf("비밀번호 입력: ");
    scanf("%s", password);

    FILE *userFile = fopen("User.csv", "a");
    FILE *timeFile = fopen("User_time.csv", "a");

    if (!userFile || !timeFile) {
        printf("파일 열기 실패\n");
        return 1;
    }

    fprintf(userFile, "%s,%s,0\n", name, password); // 로그인 여부: 0
    fprintf(timeFile, "%s,0\n", name); // 시간: 0분

    fclose(userFile);
    fclose(timeFile);

    printf("회원가입 완료!\n");
    return 0;
}

