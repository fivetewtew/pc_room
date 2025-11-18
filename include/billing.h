#ifndef BILLING_H
#define BILLING_H

// 초과 사용 요금 계산 (회원용)
// over_minutes: 초과 사용 시간(분), out_cost: 계산된 요금(원), out_units_10min: 10분 단위 개수
void calculateOveruseCharge(int over_minutes, int *out_cost, int *out_units_10min);

// 게스트 초과 사용 요금 계산
void calculateGuestOveruseCharge(int elapsed_minutes, int remain_at_login, int *out_over_minutes, int *out_cost, int *out_units_10min);

#endif
