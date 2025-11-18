// billing.c
// 요금 계산 로직을 담당합니다. 초과 사용 요금을 계산합니다.
#include "billing.h"
#include "config.h"

// 초과 사용 요금 계산 (회원용)
void calculateOveruseCharge(int over_minutes, int *out_cost, int *out_units_10min) {
    if (!out_cost || !out_units_10min) return;

    if (over_minutes <= 0) {
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    if (over_minutes < 10) {
        // 10분 미만은 무료
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    // 10분 단위로 요금 계산 (버림)
    int price_per_10min = (PRODUCT_PRICE[0] * 10) / PRODUCT_MINUTES[0];
    *out_units_10min = over_minutes / 10;
    *out_cost = (*out_units_10min) * price_per_10min;
}

// 게스트 초과 사용 요금 계산
void calculateGuestOveruseCharge(int elapsed_minutes, int remain_at_login, int *out_over_minutes, int *out_cost, int *out_units_10min) {
    if (!out_over_minutes || !out_cost || !out_units_10min) return;

    int over_minutes = elapsed_minutes - remain_at_login;
    *out_over_minutes = over_minutes;

    if (over_minutes <= 0) {
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    if (over_minutes < 10) {
        // 10분 미만은 무료
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    // 10분 단위로 요금 계산 (버림)
    int price_per_10min = (PRODUCT_PRICE[0] * 10) / PRODUCT_MINUTES[0];
    *out_units_10min = over_minutes / 10;
    *out_cost = (*out_units_10min) * price_per_10min;
}
