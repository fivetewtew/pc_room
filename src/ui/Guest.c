// Guest.c
// 손님(게스트) 시간 관리 UI. 데이터 저장/로드는 storage 모듈을 사용합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Guest.h"
#include "models.h"
#include "config.h"

#include "storage.h"

// 남은 시간 계산 (경과 시간만큼 차감)
// GuestInfo.last_time을 기준으로 경과 분을 구해 remain_time에서 차감합니다.
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
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("%d. %d분 (%d원)\n", i + 1, PRODUCT_MINUTES[i], PRODUCT_PRICE[i]);
    }
    printf("선택: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > PRODUCT_COUNT) {
        printf("잘못된 선택입니다.\n");
        return;
    }
    added_time = PRODUCT_MINUTES[choice - 1];
    price = PRODUCT_PRICE[choice - 1];

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
    saveGuestInfo(guest);

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

    if (!loadGuestInfo(guest_id, &guest)) {
        printf("손님 [%s] 정보를 찾을 수 없어 새로 생성합니다.\n", guest_id);
        strcpy(guest.id, guest_id);
        guest.remain_time = 0;
        guest.last_time = time(NULL);
        saveGuestInfo(&guest);
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


