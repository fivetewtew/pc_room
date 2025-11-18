// login.c
// 로그인/로그아웃 UI. 실제 상태 변경 및 시간 차감 로직은 auth/storage 모듈이 담당합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "login.h"
#include "storage.h"
#include "auth.h"
#include "time.h"
#include "billing.h"
#include "utils.h"

// 회원 남은 시간 조회 (로그인 중이면 경과 시간 반영)
void showRemainingTimeMenu(void) {
    // setup_console_utf8() 제거 - main()에서 이미 설정됨
    
    char input_id[MAX_LEN];
    printf("회원 ID 입력: ");
    scanf("%99s", input_id);

    int remaining = 0;
    if (getRemainingTimeWithElapsed(input_id, &remaining)) {
        printf("[%s] 회원의 남은 시간: %d분\n", input_id, remaining);
    } else {
        printf("ID를 찾을 수 없습니다.\n");
    }
}

void loginMenu(void) {
    // setup_console_utf8() 제거 - main()에서 이미 설정됨
    
    char input_id[MAX_LEN];
    printf("ID 입력 (로그인/로그아웃): ");
    scanf("%99s", input_id);

    // 회원 정보 로드
    UserRecord user;
    if (!loadUser(input_id, &user)) {
        printf("ID를 찾을 수 없습니다.\n");
        return;
    }

    if (user.is_logged_in == 0) {
        // 로그인 UI: 비밀번호 입력 후 로직에 위임
        char password_input[MAX_LEN];
        printf("비밀번호 입력: ");
        scanf("%99s", password_input);

        // 남은 시간이 없으면 로그인 차단
        {
            UserTime tcheck;
            if (!loadUserTime(user.id, &tcheck) || tcheck.minutes <= 0) {
                printf("남은 시간이 없습니다. 로그인 전에 시간을 충전해주세요.\n");
                return;
            }
        }

        UserTime t;
        if (try_login(&user, password_input, &t)) {
            printf("[%s] 로그인 성공!\n", user.id);
            printf("남은 시간: %d분\n", t.minutes);
        } else {
            printf("로그인 실패 (비밀번호가 틀렸거나 이미 로그인된 상태입니다).\n");
        }
    } else {
        // 로그아웃 UI
        int elapsed = 0;
        // 로그아웃 전 남은 시간 캡처 (초과 사용 계산용)
        int prev_remaining = 0;
        {
            UserTime before;
            if (loadUserTime(user.id, &before)) prev_remaining = before.minutes;
        }
        UserTime t;
        if (try_logout(&user, &elapsed, &t)) {
            if (elapsed > 0)
                printf("로그인 후 경과 시간: %d분\n", elapsed);
            printf("[%s] 로그아웃 완료. 남은 시간: %d분\n", user.id, t.minutes);

            // 초과 사용 요금 계산 (billing 모듈 사용)
            int over_minutes = elapsed - prev_remaining;
            if (over_minutes > 0) {
                int cost = 0, units_10min = 0;
                calculateOveruseCharge(over_minutes, &cost, &units_10min);
                
                if (cost == 0) {
                    printf("%d분 초과 사용 (10분 미만). 무료 서비스입니다.\n", over_minutes);
                } else {
                    printf("%d분 초과 사용. %d x 10분 단위로 요금이 부과됩니다 (10분 미만은 무료).\n",
                           over_minutes, units_10min);
                    printf("결제 금액: %d원\n", cost);
                }
            }
        } else {
            printf("로그아웃 실패 (로그인되지 않았거나 데이터 오류).\n");
        }
    }
}

