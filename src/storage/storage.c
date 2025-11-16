// storage.c
// CSV 기반 저장소 접근을 담당합니다.
// User.csv, User_time.csv, login_times.csv, Guest_time.csv를 일관된 방식으로 읽고/씁니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "storage.h"

// 데이터 파일 경로
#define DATA_DIR                "data"
#define PATH_USER               DATA_DIR "/User.csv"
#define PATH_USER_TEMP          DATA_DIR "/User_temp.csv"
#define PATH_USER_TIME          DATA_DIR "/User_time.csv"
#define PATH_USER_TIME_TEMP     DATA_DIR "/User_time_temp.csv"
#define PATH_LOGIN_TIMES        DATA_DIR "/login_times.csv"
#define PATH_LOGIN_TIMES_TEMP   DATA_DIR "/login_times_temp.csv"
#define PATH_GUEST              DATA_DIR "/Guest_time.csv"
#define PATH_GUEST_TEMP         DATA_DIR "/Guest_time_temp.csv"

static void ensureDataDir(void) {
    struct stat st;
    if (stat(DATA_DIR, &st) == -1) {
        // best-effort 생성
        mkdir(DATA_DIR, 0777);
    }
}

// User.csv: id,password,status
int loadUser(const char *id, UserRecord *outUser) {
    // User.csv에서 id에 해당하는 레코드를 찾아 반환
    ensureDataDir();
    FILE *file = fopen(PATH_USER, "r");
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
    ensureDataDir();
    FILE *file = fopen(PATH_USER, "r");
    FILE *temp = fopen(PATH_USER_TEMP, "w");
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
    remove(PATH_USER);
    rename(PATH_USER_TEMP, PATH_USER);
    return 1;
}

// User_time.csv: id,minutes
int loadUserTime(const char *id, UserTime *outTime) {
    // User_time.csv에서 id에 해당하는 남은 시간(minutes) 조회
    ensureDataDir();
    FILE *file = fopen(PATH_USER_TIME, "r");
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

    ensureDataDir();
    FILE *file = fopen(PATH_USER_TIME, "r");
    FILE *temp = fopen(PATH_USER_TIME_TEMP, "w");
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
    remove(PATH_USER_TIME);
    rename(PATH_USER_TIME_TEMP, PATH_USER_TIME);
    return 1;
}

// login_times.csv: id,time_t
int addLoginTime(const char *id, time_t t) {
    // login_times.csv에 (id, 로그인시각) 추가
    ensureDataDir();
    FILE *file = fopen(PATH_LOGIN_TIMES, "a");
    if (!file) return 0;
    fprintf(file, "%s,%ld\n", id, (long)t);
    fclose(file);
    return 1;
}

int popLoginTime(const char *id, time_t *out_t) {
    // login_times.csv에서 id에 해당하는 항목을 읽어오고(기억) 제거
    ensureDataDir();
    FILE *file = fopen(PATH_LOGIN_TIMES, "r");
    FILE *temp = fopen(PATH_LOGIN_TIMES_TEMP, "w");
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
    remove(PATH_LOGIN_TIMES);
    rename(PATH_LOGIN_TIMES_TEMP, PATH_LOGIN_TIMES);

    if (!found) return 0;
    if (out_t) *out_t = (time_t)login_time;
    return 1;
}

// -------------------------
// Guest_time.csv: id,remain_time,last_time
// -------------------------
int loadGuestInfo(const char *id, GuestInfo *outGuest) {
    // Guest_time.csv에서 손님 정보 조회
    ensureDataDir();
    FILE *fp = fopen(PATH_GUEST, "r");
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
    ensureDataDir();
    FILE *fp = fopen(PATH_GUEST, "r");
    FILE *temp = fopen(PATH_GUEST_TEMP, "w");
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
    remove(PATH_GUEST);
    rename(PATH_GUEST_TEMP, PATH_GUEST);
    return 1;
}


