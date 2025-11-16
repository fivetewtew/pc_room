// newUser.c
// 회원 등록 UI. 실제 저장소 접근은 storage 모듈을 사용합니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "newUser.h"
#include "storage.h"

void newUser(void) {
    char name[MAX_LEN], password[MAX_LEN];

    printf("Enter new member ID: ");
    scanf("%99s", name);

    // Check if ID already exists
    UserRecord existing;
    if (loadUser(name, &existing)) {
        printf("ID already exists.\n");
        return;
    }

    printf("Enter password: ");
    scanf("%99s", password);

    // Initialize new user/time
    UserRecord user;
    memset(&user, 0, sizeof(user));
    snprintf(user.id, sizeof(user.id), "%s", name);
    snprintf(user.password, sizeof(user.password), "%s", password);
    user.is_logged_in = 0;

    UserTime timeRec;
    memset(&timeRec, 0, sizeof(timeRec));
    snprintf(timeRec.id, sizeof(timeRec.id), "%s", name);
    timeRec.minutes = 0;

    // Save
    if (!saveUser(&user) || !saveUserTime(&timeRec)) {
        printf("Failed to save data files.\n");
        return;
    }

    printf("Member registration completed.\n");
}

