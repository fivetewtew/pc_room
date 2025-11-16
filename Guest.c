#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Guest.h"

#define MAX_LEN 100
#define FILE_NAME "Guest_time.csv"

// 손님(게스트) 시간 정보 구조체
typedef struct {
    char id[MAX_LEN];
    int remain_time;   // 남은 이용 시간(분)
    time_t last_time;  // 마지막 기준 시간 (epoch time)
} GuestInfo;

// 파일에서 손님 정보 읽기 (성공 시 1, 실패 시 0 반환)
int readGuest(const char *id, GuestInfo *guest) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char file_id[MAX_LEN];
        int remain;
        long long last;  // time_t�� ���� long long Ȥ�� long

        if (sscanf(line, "%[^,],%d,%lld", file_id, &remain, &last) == 3) {
            if (strcmp(id, file_id) == 0) {
                strcpy(guest->id, file_id);
                guest->remain_time = remain;
                guest->last_time = (time_t)last;
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

// 파일에 손님 정보 전체 갱신 (임시 파일 사용)
void writeGuest(const GuestInfo *guest) {
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!temp) {
        printf("파일 작업에 실패했습니다.\n");
        if (fp) fclose(fp);
        return;
    }

    int found = 0;
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            char file_id[MAX_LEN];
            if (sscanf(line, "%[^,],", file_id) == 1) {
                if (strcmp(file_id, guest->id) == 0) {
                    // 같은 ID면 새 정보로 덮어쓰기
                    fprintf(temp, "%s,%d,%lld\n", guest->id, guest->remain_time, (long long)guest->last_time);
                    found = 1;
                } else {
                    fputs(line, temp);
                }
            }
        }
        fclose(fp);
    }

    // 기존 파일에 동일 ID가 없으면 새로 추가
    if (!found) {
        fprintf(temp, "%s,%d,%lld\n", guest->id, guest->remain_time, (long long)guest->last_time);
    }

    fclose(temp);

    // 원본 파일 교체
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
}

// 남은 시간 계산 (경과 시간만큼 차감)
void updateRemainingTime(GuestInfo *guest) {
    time_t now = time(NULL);
    int elapsed_min = (int)((now - guest->last_time) / 60);
    if (elapsed_min > 0) {
        guest->remain_time -= elapsed_min;
        if (guest->remain_time < 0) guest->remain_time = 0;
        guest->last_time = now;
    }
}

// 손님 시간 충전 메뉴
static void chargeTime(GuestInfo *guest) {
    int choice, added_time = 0, price = 0;
    int money;

    printf("\n충전할 시간을 선택하세요:\n");
    printf("1. 1시간 (1000원)\n2. 2시간 (1800원)\n3. 3시간 (3500원)\n선택: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: added_time = 60; price = 1000; break;
        case 2: added_time = 120; price = 1800; break;
        case 3: added_time = 180; price = 3500; break;
        default:
            printf("잘못된 선택입니다.\n");
            return;
    }

    printf("투입할 금액을 입력하세요 (충전 금액 이상): ");
    scanf("%d", &money);

    if (money < price) {
        printf("금액이 부족합니다. 거래를 취소합니다.\n");
        return;
    }

    int change = money - price;
    if (change > 0)
        printf("잔돈 %d원을 거슬러드립니다.\n", change);

    // 충전 직전 경과 시간 반영
    updateRemainingTime(guest);

    guest->remain_time += added_time;
    guest->last_time = time(NULL);
    writeGuest(guest);

    printf("충전 후 남은 시간: %d분\n", guest->remain_time);
}

// 남은 시간 조회
static void showRemainingTime(GuestInfo *guest) {
    updateRemainingTime(guest);

    if (guest->remain_time <= 0) {
        printf("손님 [%s]의 남은 시간이 모두 소진되었습니다.\n", guest->id);
    } else {
        printf("손님 [%s]의 남은 시간: %d분\n", guest->id, guest->remain_time);
    }
}

void guestMenu(void) {
    char guest_id[MAX_LEN];
    GuestInfo guest;

    printf("손님 ID를 입력하세요 (처음이면 새로 등록됩니다): ");
    scanf("%s", guest_id);

    printf("\n");

    if (!readGuest(guest_id, &guest)) {
        printf("손님 [%s] 정보를 찾을 수 없어 새로 생성합니다.\n", guest_id);
        strcpy(guest.id, guest_id);
        guest.remain_time = 0;
        guest.last_time = time(NULL);
        writeGuest(&guest);
    }

    while (1) {
        printf("\n------ 손님 메뉴 ------\n");
        printf("1. 시간 충전\n2. 남은 시간 조회\n3. 뒤로가기\n");
        printf("-------------------------\n");
        printf("선택: ");

        int sel;
        scanf("%d", &sel);

        switch (sel) {
            case 1:
                chargeTime(&guest);
                break;
            case 2:
                showRemainingTime(&guest);
                break;
            case 3:
                printf("손님 메뉴를 종료합니다.\n");
                return 0;
            default:
                printf("잘못된 입력입니다.\n");
        }
    }

    return 0;
}

