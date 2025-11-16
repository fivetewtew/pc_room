#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Guest.h"

#define MAX_LEN 100
#define FILE_NAME "Guest_time.csv"

// �Խ�Ʈ ���� ����ü
typedef struct {
    char id[MAX_LEN];
    int remain_time;   // �� ���� ���� �ð�
    time_t last_time;  // ������ ��� �ð� (epoch time)
} GuestInfo;

// ���Ͽ��� �Խ�Ʈ ���� �б� (�����ϸ� 1 ��ȯ, ������ 0)
int readGuest(const char *id, GuestInfo *guest) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char file_id[MAX_LEN];
        int remain;
        long long last;  // time_t�� ���� long long Ȥ�� long

        if (sscanf(line, "%[^,],%d,%lld", file_id, &remain, &last) == 3) {
            if (strcmp(id, file_id) == 0) {
                strcpy(guest->id, file_id);
                guest->remain_time = remain;
                guest->last_time = (time_t)last;
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

// ���Ͽ� �Խ�Ʈ ���� ���� (�����)
void writeGuest(const GuestInfo *guest) {
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!temp) {
        printf("���� �۾� ����\n");
        if (fp) fclose(fp);
        return;
    }

    int found = 0;
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            char file_id[MAX_LEN];
            if (sscanf(line, "%[^,],", file_id) == 1) {
                if (strcmp(file_id, guest->id) == 0) {
                    // ������Ʈ�� ������ ����
                    fprintf(temp, "%s,%d,%lld\n", guest->id, guest->remain_time, (long long)guest->last_time);
                    found = 1;
                } else {
                    fputs(line, temp);
                }
            }
        }
        fclose(fp);
    }

    // ���� ���� ���Ͽ� ���� ID�� ���� �߰�
    if (!found) {
        fprintf(temp, "%s,%d,%lld\n", guest->id, guest->remain_time, (long long)guest->last_time);
    }

    fclose(temp);

    // ���� ���� ��ü
    remove(FILE_NAME);
    rename("temp.csv", FILE_NAME);
}

// ���� �ð� ��� (���� �ð� ���� �ǽð� ����)
void updateRemainingTime(GuestInfo *guest) {
    time_t now = time(NULL);
    int elapsed_min = (int)((now - guest->last_time) / 60);
    if (elapsed_min > 0) {
        guest->remain_time -= elapsed_min;
        if (guest->remain_time < 0) guest->remain_time = 0;
        guest->last_time = now;
    }
}

// ���� �޴� & ó��
static void chargeTime(GuestInfo *guest) {
    int choice, added_time = 0, price = 0;
    int money;

    printf("\n������ �ð��� �����ϼ���:\n");
    printf("1. 1�ð� (1000��)\n2. 2�ð� (1800��)\n3. 3�ð� (3500��)\n����: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: added_time = 60; price = 1000; break;
        case 2: added_time = 120; price = 1800; break;
        case 3: added_time = 180; price = 3500; break;
        default:
            printf("�߸��� �����Դϴ�.\n");
            return;
    }

    printf("���� �־��ּ��� (���� �ݾ� �̻�): ");
    scanf("%d", &money);

    if (money < price) {
        printf("���� �����մϴ�. ���� ���.\n");
        return;
    }

    int change = money - price;
    if (change > 0)
        printf("�ܵ� %d���� ��ȯ�մϴ�.\n", change);

    // ���� �� �ǽð� ���� ����
    updateRemainingTime(guest);

    guest->remain_time += added_time;
    guest->last_time = time(NULL);
    writeGuest(guest);

    printf("? ���� �� ���� �ð�: %d��\n", guest->remain_time);
}

// ���� �ð� ��ȸ
static void showRemainingTime(GuestInfo *guest) {
    updateRemainingTime(guest);

    if (guest->remain_time <= 0) {
        printf("��ȸ�� [%s]�� ���� �ð��� �����ϴ�.\n", guest->id);
    } else {
        printf("��ȸ�� [%s]�� ���� �ð�: %d��\n", guest->id, guest->remain_time);
    }
}

void guestMenu(void) {
    char guest_id[MAX_LEN];
    GuestInfo guest;

    printf("��ȸ�� ID�� �Է��ϼ��� (��� ��й�ȣ ����): ");
    scanf("%s", guest_id);

    printf("\n");

    if (!readGuest(guest_id, &guest)) {
        printf("��ȸ�� [%s] ������ ã�� �� ���� ���� �����մϴ�.\n", guest_id);
        strcpy(guest.id, guest_id);
        guest.remain_time = 0;
        guest.last_time = time(NULL);
        writeGuest(&guest);
    }

    while (1) {
        printf("\n------ ��ȸ�� �޴� ------\n");
        printf("1. �ð� ����\n2. ���� �ð� ��ȸ\n3. ����\n");
        printf("-------------------------\n");
        printf("����: ");

        int sel;
        scanf("%d", &sel);

        switch (sel) {
            case 1:
                chargeTime(&guest);
                break;
            case 2:
                showRemainingTime(&guest);
                break;
            case 3:
                printf("��ȸ�� �޴��� �����մϴ�.\n");
                return 0;
            default:
                printf("�߸��� �����Դϴ�.\n");
        }
    }

    return 0;
}

