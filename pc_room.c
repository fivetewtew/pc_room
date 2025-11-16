// pc_room.c (메인 메뉴)
#include <stdio.h>
#include <stdlib.h>

#include "newUser.h"
#include "login.h"
#include "charge.h"
#include "Guest.h"

int main(void) {
    char c;

    while (1) {
        printf("\n====== �޴� ======\n");
        printf("1. ȸ������\n");
        printf("2. �α���/�α׾ƿ� (ID �Է�)\n");
        printf("3. �ð� ����\n");
        printf("4. ����\n");
        printf("5. ��ȸ�� �޴�\n");   // ��ȸ�� �޴� �߰�
        printf("==================\n");
        printf("���� >> ");
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
                printf("���α׷� ����\n");
                return 0;
            case '5':
                guestMenu();  // ��ȸ�� �޴� ����
                break;
            default:
                printf("�߸��� �Է��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
    }

    return 0;
}

