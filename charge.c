// charge.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "charge.h"
#include "storage.h"

#define MAX_LEN 100

// User.csv���� ����� ���� ���� Ȯ��
static int userExists(const char* username) {
    FILE *file = fopen("User.csv", "r");
    if (!file) return 0;

    char line[MAX_LEN], user[MAX_LEN];

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],", user);
        if (strcmp(user, username) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

// User.csv���� �α��� ���� Ȯ��
static int isUserLoggedIn(const char* username) {
    FILE *file = fopen("User.csv", "r");
    if (!file) return 0;

    char line[MAX_LEN], user[MAX_LEN], pass[MAX_LEN], status[MAX_LEN];

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%[^,],%s", user, pass, status);
        if (strcmp(user, username) == 0) {
            fclose(file);
            return strcmp(status, "1") == 0;
        }
    }

    fclose(file);
    return 0;
}

// �α��� �ð� �б� �Լ� (�������� ����)
static time_t getLoginTime(const char *username) {
    FILE *file = fopen("login_times.csv", "r");
    if (!file) return 0;

    char line[MAX_LEN], user[MAX_LEN];
    long login_time = 0;

    while (fgets(line, sizeof(line), file)) {
        long t;
        sscanf(line, "%[^,],%ld", user, &t);
        if (strcmp(user, username) == 0) {
            login_time = t;
            break;
        }
    }

    fclose(file);
    return (time_t)login_time;
}

static int chargeTime(const char *username) {
    int choice, added_time = 0, price = 0, cash;

    printf("\n������ �ð��� �����ϼ���:\n");
    printf("1. 1�ð� (1000��)\n2. 2�ð� (1800��)\n3. 3�ð� (3500��)\n����: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: added_time = 60; price = 1000; break;
        case 2: added_time = 120; price = 1800; break;
        case 3: added_time = 180; price = 3500; break;
        default:
            printf("�߸��� �����Դϴ�.\n");
            return 0;
    }

    printf("���� �־��ּ���: ");
    scanf("%d", &cash);

    if (cash < price) {
        printf("���� �����մϴ�. ���� ���� �� %d�� ��ȯ�մϴ�.\n", cash);
        return 0;
    }

    printf("%d�� ���� �Ϸ�! %d�� �����˴ϴ�.\n", price, added_time);
    printf("�ܵ� %d�� ��ȯ�մϴ�.\n", cash - price);

    // ����ڰ� ���� �α��� ������ Ȯ��
    if (isUserLoggedIn(username)) {
        // �α��� ���̸� ����� �ð���ŭ ���� �� ����
        time_t login_time = getLoginTime(username);
        time_t now = time(NULL);
        
        if (login_time > 0) {
            int elapsed_min = (int)((now - login_time) / 60);
            int current_remaining = 0;
            if (!getUserTime(username, &current_remaining)) current_remaining = 0;
            
            // ����� �ð� ���� �� ����
            int updated_time = (current_remaining - elapsed_min) + added_time;
            if (updated_time < 0) updated_time = 0;
            
            setUserTime(username, updated_time);
            printf("�α��� �� ����� %d�� ���� �� %d�� �����Ǿ� �� %d�� ���ҽ��ϴ�.\n", 
                   elapsed_min, added_time, updated_time);
        } else {
            // �α��� �ð� ����� ������ �׳� ����
            int remain = 0;
            if (!getUserTime(username, &remain)) remain = 0;
            remain += added_time;
            setUserTime(username, remain);
            printf("? ���� �� ���� �ð�: %d��\n", remain);
        }
    } else {
        // �α��� ���� �ƴϸ� �׳� ����
        int remain = 0;
        if (!getUserTime(username, &remain)) remain = 0;
        remain += added_time;
        setUserTime(username, remain);
        printf("? ���� �� ���� �ð�: %d��\n", remain);
    }
    
    return 1;
}

void chargeMenu(void) {
    char input_id[MAX_LEN];
    printf("���̵� �Է�: ");
    scanf("%s", input_id);

    // ����� ���� ���� Ȯ��
    if (!userExists(input_id)) {
        printf("�߸��� ���̵��Դϴ�.\n");
        return;
    }

    if (!chargeTime(input_id)) {
        printf("���� �޴��� ���ư��ϴ�.\n");
        return;
    }

    printf("���� �Ϸ�!\n");
}
