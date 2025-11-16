// Guest.c
// Guest time management UI. Data I/O uses storage module.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Guest.h"
#include "models.h"
#include "config.h"

#include "storage.h"

// Deduct elapsed minutes from remaining time
// Based on GuestInfo.last_time
void updateRemainingTime(GuestInfo *guest) {
    time_t now = time(NULL);
    int elapsed_min = (int)((now - guest->last_time) / 60);
    if (elapsed_min > 0) {
        guest->remain_time -= elapsed_min;
        if (guest->remain_time < 0) guest->remain_time = 0;
        guest->last_time = now;
    }
}

// Charge guest time
static void chargeTime(GuestInfo *guest) {
    int choice, added_time = 0, price = 0;
    int money;

    printf("\nSelect a package to charge:\n");
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        printf("%d. %d minutes (%d KRW)\n", i + 1, PRODUCT_MINUTES[i], PRODUCT_PRICE[i]);
    }
    printf("Select: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > PRODUCT_COUNT) {
        printf("Invalid selection.\n");
        return;
    }
    added_time = PRODUCT_MINUTES[choice - 1];
    price = PRODUCT_PRICE[choice - 1];

    printf("Enter amount (>= price, KRW): ");
    scanf("%d", &money);

    if (money < price) {
        printf("Insufficient amount. Cancelling.\n");
        return;
    }

    int change = money - price;
    if (change > 0)
        printf("Change: %d KRW.\n", change);

    // 충전 직전 경과 시간 반영
    updateRemainingTime(guest);

    guest->remain_time += added_time;
    guest->last_time = time(NULL);
    saveGuestInfo(guest);

    printf("Remaining time after charge: %d minutes\n", guest->remain_time);
}

// Guest login: record session with current remaining time
static void guestLogin(GuestInfo *guest) {
    // Ensure remaining time is up to date before recording session
    updateRemainingTime(guest);
    saveGuestInfo(guest);
    time_t now = time(NULL);
    if (addGuestSession(guest->id, now, guest->remain_time)) {
        printf("Guest [%s] logged in. Remaining: %d minutes\n", guest->id, guest->remain_time);
    } else {
        printf("Failed to record guest login.\n");
    }
}

// Guest logout: compute overuse and bill per rule (>=10min billed in 10-min units, <10 free)
static void guestLogout(GuestInfo *guest) {
    time_t login_time;
    int remain_at_login = 0;
    if (!getGuestSession(guest->id, &login_time, &remain_at_login)) {
        printf("No active guest session found. Please login first.\n");
        return;
    }
    // Update remaining based on actual elapsed
    updateRemainingTime(guest);
    saveGuestInfo(guest);

    time_t now = time(NULL);
    int elapsed = (int)((now - login_time) / 60);
    printf("Elapsed since login: %d minutes\n", elapsed);

    // finalize session pop
    popGuestSession(guest->id, NULL, NULL);

    int over_minutes = elapsed - remain_at_login;
    if (over_minutes <= 0) {
        printf("No overuse. Goodbye!\n");
        // Remove guest record upon logout
        if (deleteGuestInfo(guest->id)) {
            printf("Guest record removed.\n");
        }
        return;
    }
    if (over_minutes < 10) {
        printf("Overused by %d minutes (<10). No charge (service).\n", over_minutes);
        if (deleteGuestInfo(guest->id)) {
            printf("Guest record removed.\n");
        }
        return;
    }
    int price_per_10min = (PRODUCT_PRICE[0] * 10) / PRODUCT_MINUTES[0];
    int units_10min = over_minutes / 10;
    int cost = units_10min * price_per_10min;
    printf("Overused by %d minutes. Charging %d x 10-minute unit(s).\n", over_minutes, units_10min);
    printf("Please pay: %d KRW.\n", cost);
    if (deleteGuestInfo(guest->id)) {
        printf("Guest record removed.\n");
    }
}

// Show remaining time
static void showRemainingTime(GuestInfo *guest) {
    updateRemainingTime(guest);

    if (guest->remain_time <= 0) {
        printf("Guest [%s] has no remaining time.\n", guest->id);
    } else {
        printf("Guest [%s] remaining time: %d minutes\n", guest->id, guest->remain_time);
    }
}

void guestMenu(void) {
    char guest_id[MAX_LEN];
    GuestInfo guest;

    printf("Enter guest ID (new IDs will be created): ");
    scanf("%s", guest_id);

    printf("\n");

    if (!loadGuestInfo(guest_id, &guest)) {
        printf("Guest [%s] not found. Creating a new record.\n", guest_id);
        strcpy(guest.id, guest_id);
        guest.remain_time = 0;
        guest.last_time = time(NULL);
        saveGuestInfo(&guest);
    }

    while (1) {
        printf("\n------ Guest Menu ------\n");
        printf("1. Charge time\n");
        printf("2. Show remaining time\n");
        printf("3. Login (start session)\n");
        printf("4. Logout (end session & billing)\n");
        printf("5. Back\n");
        printf("------------------------\n");
        printf("Select: ");

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
                guestLogin(&guest);
                return;
            case 4:
                guestLogout(&guest);
                break;
            case 5:
                printf("Closing guest menu.\n");
                return;
            default:
                printf("Invalid input.\n");
        }
    }

    return;
}


