// storage.c
// CSV 기반 저장소 접근을 담당합니다.
// User.csv, User_time.csv, login_times.csv, Guest_time.csv를 일관된 방식으로 읽고/씁니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage.h"

// User.csv: id,password,status
int loadUser(const char *id, UserRecord *outUser) {
    // User.csv에서 id에 해당하는 레코드를 찾아 반환
    FILE *file = fopen("User.csv", "r");
    if (!file) return 0;

    char line[256];
    UserRecord tmp;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%99[^,],%99[^,],%d",
                   tmp.id, tmp.password, &tmp.is_logged_in) == 3) {
            if (strcmp(tmp.id, id) == 0) {
                if (outUser) *outUser = tmp;
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

int saveUser(const UserRecord *user) {
    // User.csv의 해당 id 라인을 새 값으로 교체(없으면 추가)
    FILE *file = fopen("User.csv", "r");
    FILE *temp = fopen("User_temp.csv", "w");
    if (!temp) {
        if (file) fclose(file);
        return 0;
    }

    char line[256];
    UserRecord tmp;
    int found = 0;

    if (file) {
        while (fgets(line, sizeof(line), file)) {
            if (sscanf(line, "%99[^,],%99[^,],%d",
                       tmp.id, tmp.password, &tmp.is_logged_in) == 3) {
                if (strcmp(tmp.id, user->id) == 0) {
                    fprintf(temp, "%s,%s,%d\n",
                            user->id, user->password, user->is_logged_in);
                    found = 1;
                } else {
                    fputs(line, temp);
                }
            }
        }
        fclose(file);
    }

    if (!found) {
        fprintf(temp, "%s,%s,%d\n",
                user->id, user->password, user->is_logged_in);
    }

    fclose(temp);
    remove("User.csv");
    rename("User_temp.csv", "User.csv");
    return 1;
}

// User_time.csv: id,minutes
int loadUserTime(const char *id, UserTime *outTime) {
    // User_time.csv에서 id에 해당하는 남은 시간(minutes) 조회
    FILE *file = fopen("User_time.csv", "r");
    if (!file) return 0;

    char line[256];
    UserTime tmp;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%99[^,],%d", tmp.id, &tmp.minutes) == 2) {
            if (strcmp(tmp.id, id) == 0) {
                if (outTime) *outTime = tmp;
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

int saveUserTime(const UserTime *timeRec) {
    // User_time.csv의 해당 id 라인을 새 값으로 교체(없으면 추가)
    int minutes = timeRec->minutes;
    if (minutes < 0) minutes = 0;

    FILE *file = fopen("User_time.csv", "r");
    FILE *temp = fopen("User_time_temp.csv", "w");
    if (!temp) {
        if (file) fclose(file);
        return 0;
    }

    char line[256];
    UserTime tmp;
    int found = 0;

    if (file) {
        while (fgets(line, sizeof(line), file)) {
            if (sscanf(line, "%99[^,],%d", tmp.id, &tmp.minutes) == 2) {
                if (strcmp(tmp.id, timeRec->id) == 0) {
                    fprintf(temp, "%s,%d\n", timeRec->id, minutes);
                    found = 1;
                } else {
                    fputs(line, temp);
                }
            }
        }
        fclose(file);
    }

    if (!found) {
        fprintf(temp, "%s,%d\n", timeRec->id, minutes);
    }

    fclose(temp);
    remove("User_time.csv");
    rename("User_time_temp.csv", "User_time.csv");
    return 1;
}

// login_times.csv: id,time_t
int addLoginTime(const char *id, time_t t) {
    // login_times.csv에 (id, 로그인시각) 추가
    FILE *file = fopen("login_times.csv", "a");
    if (!file) return 0;
    fprintf(file, "%s,%ld\n", id, (long)t);
    fclose(file);
    return 1;
}

int popLoginTime(const char *id, time_t *out_t) {
    // login_times.csv에서 id에 해당하는 항목을 읽어오고(기억) 제거
    FILE *file = fopen("login_times.csv", "r");
    FILE *temp = fopen("login_times_temp.csv", "w");
    if (!file || !temp) {
        if (file) fclose(file);
        if (temp) fclose(temp);
        return 0;
    }

    char line[256], user[MAX_LEN];
    long login_time = 0;
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        long t;
        if (sscanf(line, "%99[^,],%ld", user, &t) == 2) {
            if (strcmp(user, id) == 0) {
                login_time = t;
                found = 1;
            } else {
                fputs(line, temp);
            }
        }
    }

    fclose(file);
    fclose(temp);
    remove("login_times.csv");
    rename("login_times_temp.csv", "login_times.csv");

    if (!found) return 0;
    if (out_t) *out_t = (time_t)login_time;
    return 1;
}

// -------------------------
// Guest_time.csv: id,remain_time,last_time
// -------------------------
int loadGuestInfo(const char *id, GuestInfo *outGuest) {
    // Guest_time.csv에서 손님 정보 조회
    FILE *fp = fopen("Guest_time.csv", "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char file_id[MAX_LEN];
        int remain;
        long long last;
        if (sscanf(line, "%99[^,],%d,%lld", file_id, &remain, &last) == 3) {
            if (strcmp(id, file_id) == 0) {
                if (outGuest) {
                    memset(outGuest, 0, sizeof(*outGuest));
                    strncpy(outGuest->id, file_id, MAX_LEN - 1);
                    outGuest->remain_time = remain;
                    outGuest->last_time = (time_t)last;
                }
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

int saveGuestInfo(const GuestInfo *guest) {
    // Guest_time.csv의 해당 id 라인을 새 값으로 교체(없으면 추가)
    FILE *fp = fopen("Guest_time.csv", "r");
    FILE *temp = fopen("Guest_time_temp.csv", "w");
    if (!temp) {
        if (fp) fclose(fp);
        return 0;
    }

    char line[256];
    int found = 0;
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            char file_id[MAX_LEN];
            if (sscanf(line, "%99[^,],", file_id) == 1) {
                if (strcmp(file_id, guest->id) == 0) {
                    fprintf(temp, "%s,%d,%lld\n",
                            guest->id, guest->remain_time, (long long)guest->last_time);
                    found = 1;
                } else {
                    fputs(line, temp);
                }
            }
        }
        fclose(fp);
    }

    if (!found) {
        fprintf(temp, "%s,%d,%lld\n",
                guest->id, guest->remain_time, (long long)guest->last_time);
    }

    fclose(temp);
    remove("Guest_time.csv");
    rename("Guest_time_temp.csv", "Guest_time.csv");
    return 1;
}


