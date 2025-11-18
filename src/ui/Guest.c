// Guest.c
// 게스트 시간 관리 UI. 데이터 입출력은 storage 모듈을 사용합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Guest.h"
#include "models.h"
#include "config.h"
#include "storage.h"
#include "time.h"
#include "billing.h"
#include "utils.h"

// time() 함수 명시적 선언 (경고 제거)
time_t time(time_t *timer);

// 게스트 시간 충전
static void chargeTime(GuestInfo *guest) {
    int choice, added_time = 0, price = 0;
    int money;

    printf("\n충전할 패키지를 선택하세요:\n");
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

    printf("금액 입력 (가격 이상, 원): ");
    scanf("%d", &money);

    if (money < price) {
        printf("금액이 부족합니다. 취소합니다.\n");
        return;
    }

    int change = money - price;
    if (change > 0)
        printf("거스름돈: %d원\n", change);

    // 충전 직전 경과 시간 반영 (time 모듈 사용)
    updateGuestRemainingTime(guest);

    guest->remain_time += added_time;
    guest->last_time = time(NULL);
    saveGuestInfo(guest);

    printf("충전 후 남은 시간: %d분\n", guest->remain_time);
}

// 게스트 로그인: 현재 남은 시간으로 세션 기록
static void guestLogin(GuestInfo *guest) {
    // 세션 기록 전 남은 시간 최신화 (time 모듈 사용)
    updateGuestRemainingTime(guest);
    saveGuestInfo(guest);
    time_t now = time(NULL);
    if (addGuestSession(guest->id, now, guest->remain_time)) {
        printf("게스트 [%s] 로그인 완료. 남은 시간: %d분\n", guest->id, guest->remain_time);
    } else {
        printf("게스트 로그인 기록에 실패했습니다.\n");
    }
}

// 게스트 로그아웃: 초과 사용 계산 및 규칙에 따른 요금 정산 (>=10분은 10분 단위, <10분은 무료)
static void guestLogout(GuestInfo *guest) {
    time_t login_time;
    int remain_at_login = 0;
    if (!getGuestSession(guest->id, &login_time, &remain_at_login)) {
        printf("활성 게스트 세션이 없습니다. 먼저 로그인해주세요.\n");
        return;
    }
    
    // 실제 경과 시간 기반 남은 시간 업데이트 (time 모듈 사용)
    updateGuestRemainingTime(guest);
    saveGuestInfo(guest);

    time_t now = time(NULL);
    int elapsed = (int)((now - login_time) / 60);
    printf("로그인 후 경과 시간: %d분\n", elapsed);

    // 세션 종료 처리
    popGuestSession(guest->id, NULL, NULL);

    // 초과 사용 요금 계산 (billing 모듈 사용)
    int over_minutes = 0, cost = 0, units_10min = 0;
    calculateGuestOveruseCharge(elapsed, remain_at_login, &over_minutes, &cost, &units_10min);

    if (over_minutes <= 0) {
        printf("초과 사용 없음. 안녕히 가세요!\n");
        if (deleteGuestInfo(guest->id)) {
            printf("게스트 기록이 삭제되었습니다.\n");
        }
        return;
    }

    if (cost == 0) {
        printf("%d분 초과 사용 (10분 미만). 무료 서비스입니다.\n", over_minutes);
        if (deleteGuestInfo(guest->id)) {
            printf("게스트 기록이 삭제되었습니다.\n");
        }
        return;
    }

    printf("%d분 초과 사용. %d x 10분 단위로 요금이 부과됩니다.\n", over_minutes, units_10min);
    printf("결제 금액: %d원\n", cost);
    if (deleteGuestInfo(guest->id)) {
        printf("게스트 기록이 삭제되었습니다.\n");
    }
}

// 남은 시간 표시
static void showRemainingTime(GuestInfo *guest) {
    // 경과 시간 계산 및 차감 (time 모듈 사용)
    updateGuestRemainingTime(guest);

    if (guest->remain_time <= 0) {
        printf("게스트 [%s]의 남은 시간이 없습니다.\n", guest->id);
    } else {
        printf("게스트 [%s]의 남은 시간: %d분\n", guest->id, guest->remain_time);
    }
}

void guestMenu(void) {
    
    char guest_id[MAX_LEN];
    GuestInfo guest;

    printf("게스트 ID 입력 (새 ID는 자동 생성됩니다): ");
    scanf("%s", guest_id);

    printf("\n");

    if (!loadGuestInfo(guest_id, &guest)) {
        printf("게스트 [%s]를 찾을 수 없습니다. 새 기록을 생성합니다.\n", guest_id);
        strcpy(guest.id, guest_id);
        guest.remain_time = 0;
        guest.last_time = time(NULL);
        saveGuestInfo(&guest);
    }

    while (1) {
        printf("\n------ 게스트 메뉴 ------\n");
        printf("1. 시간 충전\n");
        printf("2. 남은 시간 조회\n");
        printf("3. 로그인 (세션 시작)\n");
        printf("4. 로그아웃 (세션 종료 및 요금 정산)\n");
        printf("5. 돌아가기\n");
        printf("------------------------\n");
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
                guestLogin(&guest);
                return;
            case 4:
                guestLogout(&guest);
                break;
            case 5:
                printf("게스트 메뉴를 종료합니다.\n");
                return;
            default:
                printf("잘못된 입력입니다.\n");
        }
    }

    return;
}


