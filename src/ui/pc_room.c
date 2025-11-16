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

    // 로케일/콘솔 인코딩을 UTF-8로 설정
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    while (1) {
        // 메인 메뉴 출력
        printf(u8"\n====== 메뉴 ======\n");
        printf(u8"1. 회원 등록\n");
        printf(u8"2. 로그인/로그아웃 (ID 입력)\n");
        printf(u8"3. 시간 충전\n");
        printf(u8"4. 종료\n");
        printf(u8"5. 손님 메뉴\n");   // 손님 메뉴
        printf(u8"==================\n");
        printf(u8"선택 >> ");
        scanf(" %c", &c);

        switch (c) {
            case '1':
                // 회원 등록
                newUser();
                break;
            case '2':
                // 로그인/로그아웃
                loginMenu();
                break;
            case '3':
                // 회원 시간 충전
                chargeMenu();
                break;
            case '4':
                printf(u8"프로그램을 종료합니다.\n");
                return 0;
            case '5':
                // 손님(게스트) 시간 관리
                guestMenu();
                break;
            default:
                printf(u8"잘못 입력하셨습니다. 다시 시도해주세요.\n");
        }
    }

    return 0;
}


