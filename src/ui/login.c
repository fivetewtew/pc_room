// login.c
// 로그인/로그아웃 UI. 실제 상태 변경 및 시간 차감 로직은 auth/storage 모듈이 담당합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "login.h"
#include "storage.h"
#include "auth.h"
#include "config.h"

// Show remaining time for a member (considering elapsed time if logged in)
void showRemainingTimeMenu(void) {
    char input_id[MAX_LEN];
    printf("Enter member ID: ");
    scanf("%99s", input_id);

    UserRecord user;
    if (!loadUser(input_id, &user)) {
        printf("ID not found.\n");
        return;
    }

    UserTime t;
    int has_time = loadUserTime(user.id, &t);
    if (!has_time) {
        t.minutes = 0;
        snprintf(t.id, sizeof(t.id), "%s", user.id);
    }

    int remaining = t.minutes;
    if (user.is_logged_in) {
        time_t login_time;
        if (getLoginTime(user.id, &login_time)) {
            time_t now = time(NULL);
            int elapsed = (int)((now - login_time) / 60);
            if (elapsed > 0) {
                remaining -= elapsed;
                if (remaining < 0) remaining = 0;
            }
        }
    }

    printf("Remaining time for [%s]: %d minutes\n", user.id, remaining);
}

void loginMenu(void) {
    char input_id[MAX_LEN];
    printf("Enter ID (login/logout): ");
    scanf("%99s", input_id);

    // Load user info
    UserRecord user;
    if (!loadUser(input_id, &user)) {
        printf("ID not found.\n");
        return;
    }

    if (user.is_logged_in == 0) {
        // Login UI: ask password and delegate to logic
        char password_input[MAX_LEN];
        printf("Enter password: ");
        scanf("%99s", password_input);

        // Block login if no remaining time
        {
            UserTime tcheck;
            if (!loadUserTime(user.id, &tcheck) || tcheck.minutes <= 0) {
                printf("No remaining time. Please purchase time before logging in.\n");
                return;
            }
        }

        UserTime t;
        if (try_login(&user, password_input, &t)) {
            printf("[%s] Login successful!\n", user.id);
            printf("Remaining time: %d minutes\n", t.minutes);
        } else {
            printf("Login failed (wrong password or already logged in).\n");
        }
    } else {
        // Logout UI
        int elapsed = 0;
        // capture remaining time before logout to compute overuse
        int prev_remaining = 0;
        {
            UserTime before;
            if (loadUserTime(user.id, &before)) prev_remaining = before.minutes;
        }
        UserTime t;
        if (try_logout(&user, &elapsed, &t)) {
            if (elapsed > 0)
                printf("Elapsed time since login: %d minutes\n", elapsed);
            printf("[%s] Logout complete. Remaining time: %d minutes\n", user.id, t.minutes);

            // If elapsed time exceeds remaining time at login, bill per 10-minute units (under 10 minutes is free)
            int over_minutes = elapsed - prev_remaining;
            if (over_minutes > 0) {
                if (over_minutes < 10) {
                    printf("Overused by %d minutes (<10). No charge (service).\n", over_minutes);
                } else {
                    // Price per 10 minutes is derived from the 60-minute pack price
                    int price_per_10min = (PRODUCT_PRICE[0] * 10) / PRODUCT_MINUTES[0]; // floor
                    int units_10min = over_minutes / 10; // floor units of 10 min
                    int cost = units_10min * price_per_10min;
                    printf("Overused by %d minutes. Charging %d x 10-minute unit(s) (rounded down; <10 min free).\n",
                           over_minutes, units_10min);
                    printf("Please pay: %d KRW.\n", cost);
                }
            }
        } else {
            printf("Logout failed (not logged in or data error).\n");
        }
    }
}

