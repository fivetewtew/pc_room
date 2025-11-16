// login.c
// 로그인/로그아웃 UI. 실제 상태 변경 및 시간 차감 로직은 auth/storage 모듈이 담당합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "login.h"
#include "storage.h"
#include "auth.h"

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
        UserTime t;
        if (try_logout(&user, &elapsed, &t)) {
            if (elapsed > 0)
                printf("Elapsed time since login: %d minutes\n", elapsed);
            printf("[%s] Logout complete. Remaining time: %d minutes\n", user.id, t.minutes);
        } else {
            printf("Logout failed (not logged in or data error).\n");
        }
    }
}

