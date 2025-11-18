#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#endif

// Windows 콘솔 UTF-8 설정 (한글 깨짐 방지)
static inline void setup_console_utf8(void) {
    #ifdef _WIN32
    system("chcp 65001 > nul");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
}

#endif
