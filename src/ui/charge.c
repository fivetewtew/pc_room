// charge.c
// 회원 시간 충전 UI. 남은 시간 읽기/저장은 storage 모듈을 사용합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "charge.h"
#include "storage.h"
#include "config.h"

// 회원 로그인 관련 정보는 storage/auth 모듈이 담당합니다.
// 이 파일은 충전 UI 흐름만 담당합니다.

static int chargeTime(const char *username) {
    int choice = 0, added_time = 0, price = 0, cash;

    printf("\nSelect a package to charge:\n");
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("%d. %d minutes (%d KRW)\n", i + 1, PRODUCT_MINUTES[i], PRODUCT_PRICE[i]);
    }
    printf("Select: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > PRODUCT_COUNT) {
        printf("Invalid selection.\n");
        return 0;
    }
    added_time = PRODUCT_MINUTES[choice - 1];
    price = PRODUCT_PRICE[choice - 1];

    printf("Enter amount (KRW): ");
    scanf("%d", &cash);

    if (cash < price) {
        printf("Insufficient amount. Returning %d KRW.\n", cash);
        return 0;
    }

    printf("Paid %d KRW. %d minutes will be added.\n", price, added_time);
    printf("Change: %d KRW.\n", cash - price);

    // 로그인 여부와 무관하게 항상 현재 남은 시간을 기준으로 충전
    UserTime t;
    if (!loadUserTime(username, &t)) {
        memset(&t, 0, sizeof(t));
        snprintf(t.id, sizeof(t.id), "%s", username);
    }

    t.minutes += added_time;
    if (!saveUserTime(&t)) {
        printf("시간 정보를 저장하는 중 오류가 발생했습니다.\n");
        return 0;
    }

    printf("Remaining time after charge: %d minutes\n", t.minutes);
    
    return 1;
}

void chargeMenu(void) {
    char input_id[MAX_LEN];
    printf("Enter member ID to charge: ");
    scanf("%s", input_id);

    // 회원 존재 여부 확인 (storage를 통해 검사)
    {
        UserRecord tmp;
        if (!loadUser(input_id, &tmp)) {
            printf("ID does not exist.\n");
            return;
        }
    }

    if (!chargeTime(input_id)) {
        printf("Charge cancelled.\n");
        return;
    }

    printf("Charge completed.\n");
}


