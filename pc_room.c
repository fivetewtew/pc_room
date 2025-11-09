// gpt피시방.c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char c;

    while (1) {
        printf("\n====== 메뉴 ======\n");
        printf("1. 회원가입\n");
        printf("2. 로그인/로그아웃 (ID 입력)\n");
        printf("3. 시간 충전\n");
        printf("4. 종료\n");
        printf("5. 비회원 메뉴\n");   // 비회원 메뉴 추가
        printf("==================\n");
        printf("선택 >> ");
        scanf(" %c", &c);

        switch (c) {
            case '1':
                system("newUser");
                break;
            case '2':
                system("login");
                break;
            case '3':
                system("charge");
                break;
            case '4':
                printf("프로그램 종료\n");
                return 0;
            case '5':
                system("Guest");  // 비회원 메뉴 실행
                break;
            default:
                printf("잘못된 입력입니다. 다시 시도하세요.\n");
        }
    }

    return 0;
}

