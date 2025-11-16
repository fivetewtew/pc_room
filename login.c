// login.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "login.h"
#include "storage.h"

void loginMenu(void) {
    char input_id[MAX_LEN];
    printf("���̵� �Է� (�α���/�α׾ƿ�): ");
    scanf("%99s", input_id);

    UserRecord user;
    if (!loadUser(input_id, &user)) {
        printf("�ش� ID�� ã�� �� �����ϴ�.\n");
        return;
    }

    if (user.is_logged_in == 0) {
        // �α���
        char password_input[MAX_LEN];
        printf("��й�ȣ �Է�: ");
        scanf("%99s", password_input);

        if (strcmp(password_input, user.password) == 0) {
            user.is_logged_in = 1;
            if (!saveUser(&user)) {
                printf("���� ���� ����\n");
                return;
            }

            printf("[%s] �α��� ����!\n", user.id);
            addLoginTime(user.id, time(NULL));  // �α��� �ð� ���

            // ���� �ð� ���
            UserTime t;
            if (loadUserTime(user.id, &t)) {
                printf("? ���� �ð�: %d��\n", t.minutes);
            }
        } else {
            printf("��й�ȣ�� Ʋ�Ƚ��ϴ�.\n");
        }
    } else {
        // �α׾ƿ� - ��� �ð� ����
        time_t login_time;
        int has_login = popLoginTime(user.id, &login_time);
        time_t now = time(NULL);
        int elapsed_min = 0;

        if (has_login) {
            elapsed_min = (int)((now - login_time) / 60);
            printf("�α��� �� ��� �ð�: %d��\n", elapsed_min);
        } else {
            printf("�α��� �ð��� ��ϵ��� �ʾҽ��ϴ�.\n");
        }

        UserTime t;
        int has_time = loadUserTime(user.id, &t);
        if (!has_time) {
            memset(&t, 0, sizeof(t));
            strncpy(t.id, user.id, MAX_LEN - 1);
        }

        int updated = t.minutes - elapsed_min;
        if (updated < 0) updated = 0;
        t.minutes = updated;
        saveUserTime(&t);

        user.is_logged_in = 0;
        saveUser(&user);

        printf("[%s] �α׾ƿ� �Ϸ�! ���� �ð�: %d��\n", user.id, updated);
    }
}
