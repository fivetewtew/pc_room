// billing.c
// 요금 계산 로직을 담당합니다. 초과 사용 요금을 계산합니다.
#include "billing.h"
#include "config.h"

/**
 * 회원 초과 사용 요금 계산
 * 
 * @param over_minutes 초과 사용 시간(분)
 * @param out_cost 계산된 요금을 반환할 포인터
 * @param out_units_10min 계산된 10분 단위 개수를 반환할 포인터
 * 
 * 요금 정책:
 * - 10분 미만: 무료
 * - 10분 이상: 10분 단위로 요금 계산 (나머지 분은 무료)
 * 
 * 예시:
 * - 5분 초과: 무료 (0원)
 * - 15분 초과: 1단위 (10분당 요금)
 * - 25분 초과: 2단위 (20분당 요금, 5분은 무료)
 */
void calculateOveruseCharge(int over_minutes, int *out_cost, int *out_units_10min) {
    if (!out_cost || !out_units_10min) return;

    // 초과 사용 시간이 없으면 요금 없음
    if (over_minutes <= 0) {
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    // 10분 미만은 무료 서비스
    if (over_minutes < 10) {
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    // 10분 단위로 요금 계산 (나머지 분은 버림 처리)
    // 예: 25분 → 2단위 (20분만 요금 부과, 5분은 무료)
    int price_per_10min = (PRODUCT_PRICE[0] * 10) / PRODUCT_MINUTES[0];
    *out_units_10min = over_minutes / 10;  // 10분 단위 개수 (버림)
    *out_cost = (*out_units_10min) * price_per_10min;  // 총 요금
}

/**
 * 게스트 초과 사용 요금 계산
 * 
 * @param elapsed_minutes 로그인 후 경과 시간(분)
 * @param remain_at_login 로그인 시 남아있던 시간(분)
 * @param out_over_minutes 계산된 초과 사용 시간(분)을 반환할 포인터
 * @param out_cost 계산된 요금을 반환할 포인터
 * @param out_units_10min 계산된 10분 단위 개수를 반환할 포인터
 * 
 * 동작 과정:
 * 1. 초과 사용 시간 계산: 경과 시간 - 로그인 시 남은 시간
 * 2. 10분 미만이면 무료
 * 3. 10분 이상이면 10분 단위로 요금 계산
 * 
 * 예시:
 * - 로그인 시 30분 남음, 35분 사용 → 5분 초과 (무료)
 * - 로그인 시 20분 남음, 45분 사용 → 25분 초과 (2단위 요금)
 */
void calculateGuestOveruseCharge(int elapsed_minutes, int remain_at_login, int *out_over_minutes, int *out_cost, int *out_units_10min) {
    if (!out_over_minutes || !out_cost || !out_units_10min) return;

    // 초과 사용 시간 계산: 실제 사용 시간 - 로그인 시 남은 시간
    int over_minutes = elapsed_minutes - remain_at_login;
    *out_over_minutes = over_minutes;

    // 초과 사용 시간이 없으면 요금 없음
    if (over_minutes <= 0) {
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    // 10분 미만은 무료 서비스
    if (over_minutes < 10) {
        *out_cost = 0;
        *out_units_10min = 0;
        return;
    }

    // 10분 단위로 요금 계산 (나머지 분은 버림 처리)
    int price_per_10min = (PRODUCT_PRICE[0] * 10) / PRODUCT_MINUTES[0];
    *out_units_10min = over_minutes / 10;  // 10분 단위 개수 (버림)
    *out_cost = (*out_units_10min) * price_per_10min;  // 총 요금
}
