// newUser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "newUser.h"
#include "storage.h"

void newUser(void) {
    char name[MAX_LEN], password[MAX_LEN];

    printf("����� �̸� �Է�: ");
    scanf("%99s", name);

    // 이미 존재하는지 확인
    UserRecord existing;
    if (loadUser(name, &existing)) {
        printf("�̹� �����ϴ� ����� �̸��Դϴ�.\n");
        return;
    }

    printf("��й�ȣ �Է�: ");
    scanf("%99s", password);

    UserRecord user;
    memset(&user, 0, sizeof(user));
    strncpy(user.id, name, MAX_LEN - 1);
    strncpy(user.password, password, MAX_LEN - 1);
    user.is_logged_in = 0;

    UserTime timeRec;
    memset(&timeRec, 0, sizeof(timeRec));
    strncpy(timeRec.id, name, MAX_LEN - 1);
    timeRec.minutes = 0;

    if (!saveUser(&user) || !saveUserTime(&timeRec)) {
        printf("���� ���� ����\n");
        return;
    }

    printf("ȸ������ �Ϸ�!\n");
}
