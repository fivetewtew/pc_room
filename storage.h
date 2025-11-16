#ifndef STORAGE_H
#define STORAGE_H

#include <time.h>
#include "models.h"

// User.csv: id,password,status(0/1)
int loadUser(const char *id, UserRecord *outUser);       // 1=성공,0=없음
int saveUser(const UserRecord *user);                    // 1=성공,0=실패

// User_time.csv: id,minutes
int loadUserTime(const char *id, UserTime *outTime);     // 1=성공,0=없음
int saveUserTime(const UserTime *timeRec);               // 1=성공,0=실패

// login_times.csv: id,time_t
int addLoginTime(const char *id, time_t t);              // 1=성공,0=실패
int popLoginTime(const char *id, time_t *out_t);         // 1=성공,0=없음

#endif



