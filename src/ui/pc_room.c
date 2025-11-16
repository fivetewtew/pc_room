// pc_room.c
// 메인 메뉴(UI) 전용 파일입니다.
// 각 기능은 모듈의 공개 함수(예: newUser, loginMenu, chargeMenu, guestMenu)를 호출합니다.
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "newUser.h"
#include "login.h"
#include "charge.h"
#include "Guest.h"

int main(void) {
    char c;

    // Locale setup (Git Bash/mintty is UTF-8 by default)
    if (!setlocale(LC_ALL, "")) {
        // Fallback to UTF-8
        setlocale(LC_ALL, "ko_KR.UTF-8");
    }
#ifdef _WIN32
    // Windows console only (no effect in Git Bash/mintty)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    while (1) {
        // 메인 메뉴 출력
        printf("\n====== Menu ======\n");
        printf("1. Register member\n");
        printf("2. Login/Logout (enter ID)\n");
        printf("3. Charge time\n");
        printf("4. Guest menu\n");
        printf("5. Show member remaining time\n");
        printf("6. Exit\n");
        printf("==================\n");
        printf("Select >> ");
        scanf(" %c", &c);

        switch (c) {
            case '1':
                // Register member
                newUser();
                break;
            case '2':
                // Login/Logout
                loginMenu();
                break;
            case '3':
                // Charge member time
                chargeMenu();
                break;
            case '4':
                // Guest time management
                guestMenu();
                break;
            case '5':
                // Show member remaining time
                showRemainingTimeMenu();
                break;
            case '6':
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid input. Please try again.\n");
        }
    }

    return 0;
}


