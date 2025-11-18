// pc_room.c
// 메인 메뉴(UI) 전용 파일입니다.
// 각 기능은 모듈의 공개 함수(예: newUser, loginMenu, chargeMenu, guestMenu)를 호출합니다.
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "newUser.h"
#include "login.h"
#include "charge.h"
#include "Guest.h"

// Windows 콘솔 UTF-8 설정 (한글 깨짐 방지) - test_ui.c와 동일한 방식
void setup_console_utf8(void) {
    #ifdef _WIN32
    // chcp 65001 명령으로 UTF-8 코드 페이지 설정
    system("chcp 65001 > nul");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
}

int main(void) {
    char c;

    // Windows 콘솔 UTF-8 설정 (한글 깨짐 방지) - test_ui.c와 동일한 방식
    setup_console_utf8();
    
    // setlocale 제거 - test_ui.c에는 없음

    while (1) {
        // 메인 메뉴 출력
        printf("\n====== 메뉴 ======\n");
        printf("1. 회원 등록\n");
        printf("2. 로그인/로그아웃 (ID 입력)\n");
        printf("3. 시간 충전\n");
        printf("4. 게스트 메뉴\n");
        printf("5. 회원 남은 시간 조회\n");
        printf("6. 종료\n");
        printf("==================\n");
        printf("선택 >> ");
        fflush(stdout);
        scanf(" %c", &c);

        switch (c) {
            case '1':
                newUser();
                break;
            case '2':
                loginMenu();
                break;
            case '3':
                chargeMenu();
                break;
            case '4':
                guestMenu();
                break;
            case '5':
                showRemainingTimeMenu();
                break;
            case '6':
                printf("프로그램을 종료합니다.\n");
                return 0;
            default:
                printf("잘못된 입력입니다. 다시 시도해주세요.\n");
        }
    }

    return 0;
}


