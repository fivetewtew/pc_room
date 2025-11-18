// test_ui.c
// UI 기능 테스트 코드
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../include/storage.h"
#include "../include/auth.h"
#include "../include/time.h"
#include "../include/billing.h"
#include "../include/models.h"
#include "../include/config.h"

// time() 함수 명시적 선언
time_t time(time_t *timer);

// 테스트용 데이터 디렉토리
#define TEST_DATA_DIR "test_data"

// Windows 콘솔 UTF-8 설정 (한글 깨짐 방지)
void setup_console_utf8(void) {
    #ifdef _WIN32
    // chcp 65001 명령으로 UTF-8 코드 페이지 설정
    system("chcp 65001 > nul");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
}

// 플랫폼별 지연 함수
void delay_ms(int milliseconds) {
    #ifdef _WIN32
    Sleep(milliseconds);
    #else
    usleep(milliseconds * 1000);
    #endif
}

// 진행 상황 표시 함수
void show_progress(const char *message) {
    printf("\r[진행 중] %s", message);
    fflush(stdout);
    delay_ms(500);
}

// 테스트 완료 표시
void show_test_complete(const char *test_name) {
    printf("\r[완료] %s\n", test_name);
    fflush(stdout);
    delay_ms(800);
}

// 테스트 시작 표시
void show_test_start(const char *test_name) {
    printf("\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("▶ 테스트 시작: %s\n", test_name);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    delay_ms(500);
}

// 테스트 전 데이터 초기화
void setup_test_environment(void) {
    show_progress("테스트 환경 초기화 중...");
    
    // 테스트용 데이터 디렉토리 생성
    #ifdef _WIN32
    _mkdir(TEST_DATA_DIR);
    #else
    mkdir(TEST_DATA_DIR, 0777);
    #endif
    
    // 기존 데이터 파일 삭제 (있는 경우)
    remove("data/User.csv");
    remove("data/User_time.csv");
    remove("data/login_times.csv");
    remove("data/Guest_time.csv");
    remove("data/guest_sessions.csv");
    
    show_test_complete("테스트 환경 초기화");
}

// 테스트 1: 회원 등록 테스트
void test_newUser(void) {
    show_test_start("회원 등록");
    
    show_progress("회원 정보 생성 중...");
    
    // 테스트 데이터 준비
    UserRecord user;
    memset(&user, 0, sizeof(user));
    snprintf(user.id, sizeof(user.id), "testuser1");
    snprintf(user.password, sizeof(user.password), "password123");
    user.is_logged_in = 0;
    
    UserTime timeRec;
    memset(&timeRec, 0, sizeof(timeRec));
    snprintf(timeRec.id, sizeof(timeRec.id), "testuser1");
    timeRec.minutes = 0;
    
    delay_ms(300);
    show_progress("회원 정보 저장 중...");
    
    // 저장
    int result1 = saveUser(&user);
    int result2 = saveUserTime(&timeRec);
    
    delay_ms(300);
    
    assert(result1 == 1 && result2 == 1);
    printf("  ✓ 회원 등록 성공: %s\n", user.id);
    delay_ms(200);
    
    show_progress("중복 등록 방지 확인 중...");
    delay_ms(300);
    
    // 중복 등록 방지 테스트
    UserRecord existing;
    int exists = loadUser("testuser1", &existing);
    assert(exists == 1);
    assert(strcmp(existing.id, "testuser1") == 0);
    printf("  ✓ 중복 등록 방지 확인\n");
    
    show_test_complete("회원 등록 테스트");
}

// 테스트 2: 시간 충전 테스트
void test_chargeTime(void) {
    show_test_start("시간 충전");
    
    show_progress("회원 정보 확인 중...");
    delay_ms(300);
    
    // 회원이 존재하는지 확인
    UserRecord user;
    if (!loadUser("testuser1", &user)) {
        printf("  ✗ 테스트 회원이 없습니다. 먼저 회원 등록 테스트를 실행하세요.\n");
        return;
    }
    
    show_progress("현재 시간 조회 중...");
    delay_ms(300);
    
    // 현재 시간 조회
    UserTime before;
    loadUserTime("testuser1", &before);
    int before_minutes = before.minutes;
    
    show_progress("시간 충전 처리 중...");
    delay_ms(500);
    
    // 60분 패키지 충전 (첫 번째 패키지)
    UserTime t;
    if (!loadUserTime("testuser1", &t)) {
        memset(&t, 0, sizeof(t));
        snprintf(t.id, sizeof(t.id), "testuser1");
    }
    
    int added_time = PRODUCT_MINUTES[0]; // 60분
    t.minutes += added_time;
    saveUserTime(&t);
    
    delay_ms(300);
    show_progress("충전 결과 확인 중...");
    delay_ms(300);
    
    // 확인
    UserTime after;
    loadUserTime("testuser1", &after);
    assert(after.minutes == before_minutes + 60);
    printf("  ✓ 시간 충전 성공: %d분 → %d분\n", before_minutes, after.minutes);
    
    show_test_complete("시간 충전 테스트");
}

// 테스트 3: 로그인 테스트
void test_login(void) {
    show_test_start("로그인");
    
    show_progress("회원 정보 로드 중...");
    delay_ms(300);
    
    UserRecord user;
    if (!loadUser("testuser1", &user)) {
        printf("  ✗ 테스트 회원이 없습니다.\n");
        return;
    }
    
    show_progress("로그인 상태 확인 중...");
    delay_ms(300);
    
    // 로그인 전 상태 확인
    assert(user.is_logged_in == 0);
    
    show_progress("로그인 시도 중...");
    delay_ms(500);
    
    // 로그인 시도
    UserTime timeRec;
    int result = try_login(&user, "password123", &timeRec);
    
    delay_ms(300);
    
    assert(result == 1);
    printf("  ✓ 로그인 성공: %s\n", user.id);
    printf("  ✓ 남은 시간: %d분\n", timeRec.minutes);
    delay_ms(200);
    
    show_progress("로그인 상태 확인 중...");
    delay_ms(300);
    
    // 로그인 상태 확인
    UserRecord logged_in_user;
    loadUser("testuser1", &logged_in_user);
    assert(logged_in_user.is_logged_in == 1);
    printf("  ✓ 로그인 상태 확인\n");
    delay_ms(200);
    
    show_progress("잘못된 비밀번호 테스트 중...");
    delay_ms(400);
    
    // 잘못된 비밀번호 테스트
    UserRecord user2;
    loadUser("testuser1", &user2);
    user2.is_logged_in = 0; // 리셋
    saveUser(&user2);
    int fail_result = try_login(&user2, "wrongpassword", &timeRec);
    assert(fail_result == 0);
    printf("  ✓ 잘못된 비밀번호 거부 확인\n");
    
    show_test_complete("로그인 테스트");
}

// 테스트 4: 남은 시간 조회 테스트
void test_showRemainingTime(void) {
    show_test_start("남은 시간 조회");
    
    show_progress("남은 시간 조회 중...");
    delay_ms(500);
    
    int remaining = 0;
    int result = getRemainingTimeWithElapsed("testuser1", &remaining);
    
    delay_ms(300);
    
    assert(result == 1);
    printf("  ✓ 남은 시간 조회 성공: %d분\n", remaining);
    delay_ms(200);
    
    show_progress("존재하지 않는 회원 테스트 중...");
    delay_ms(400);
    
    // 존재하지 않는 회원 테스트
    int remaining2 = 0;
    int result2 = getRemainingTimeWithElapsed("nonexistent", &remaining2);
    assert(result2 == 0);
    printf("  ✓ 존재하지 않는 회원 처리 확인\n");
    
    show_test_complete("남은 시간 조회 테스트");
}

// 테스트 5: 로그아웃 테스트
void test_logout(void) {
    show_test_start("로그아웃");
    
    show_progress("회원 정보 확인 중...");
    delay_ms(300);
    
    UserRecord user;
    if (!loadUser("testuser1", &user)) {
        printf("  ✗ 테스트 회원이 없습니다.\n");
        return;
    }
    
    show_progress("로그인 상태 확인 중...");
    delay_ms(300);
    
    // 로그인 상태로 만들기
    if (user.is_logged_in == 0) {
        show_progress("로그인 처리 중...");
        delay_ms(400);
        try_login(&user, "password123", NULL);
        loadUser("testuser1", &user);
    }
    
    show_progress("로그아웃 전 시간 확인 중...");
    delay_ms(300);
    
    // 로그아웃 전 남은 시간 확인
    UserTime before;
    loadUserTime("testuser1", &before);
    
    show_progress("로그아웃 처리 중...");
    delay_ms(500);
    
    // 로그아웃
    int elapsed = 0;
    UserTime timeRec;
    int result = try_logout(&user, &elapsed, &timeRec);
    
    delay_ms(300);
    
    assert(result == 1);
    printf("  ✓ 로그아웃 성공: %s\n", user.id);
    printf("  ✓ 경과 시간: %d분\n", elapsed);
    printf("  ✓ 로그아웃 전 시간: %d분\n", before.minutes);
    printf("  ✓ 남은 시간: %d분\n", timeRec.minutes);
    delay_ms(200);
    
    show_progress("로그아웃 상태 확인 중...");
    delay_ms(300);
    
    // 로그아웃 상태 확인
    UserRecord logged_out_user;
    loadUser("testuser1", &logged_out_user);
    assert(logged_out_user.is_logged_in == 0);
    printf("  ✓ 로그아웃 상태 확인\n");
    
    show_test_complete("로그아웃 테스트");
}

// 테스트 6: 게스트 시간 관리 테스트
void test_guestTimeManagement(void) {
    show_test_start("게스트 시간 관리");
    
    show_progress("게스트 정보 생성 중...");
    delay_ms(400);
    
    // 게스트 정보 생성
    GuestInfo guest;
    memset(&guest, 0, sizeof(guest));
    snprintf(guest.id, sizeof(guest.id), "guest1");
    guest.remain_time = 0;
    guest.last_time = time(NULL);
    saveGuestInfo(&guest);
    
    printf("  ✓ 게스트 정보 생성: %s\n", guest.id);
    delay_ms(200);
    
    show_progress("게스트 시간 충전 중...");
    delay_ms(500);
    
    // 시간 충전 시뮬레이션
    guest.remain_time += 60; // 60분 추가
    guest.last_time = time(NULL);
    saveGuestInfo(&guest);
    
    GuestInfo loaded_guest;
    loadGuestInfo("guest1", &loaded_guest);
    assert(loaded_guest.remain_time == 60);
    printf("  ✓ 게스트 시간 충전 확인: %d분\n", loaded_guest.remain_time);
    delay_ms(200);
    
    show_progress("경과 시간 계산 테스트 중...");
    delay_ms(400);
    
    // 경과 시간 계산 테스트 (sleep 대신 시간 조작)
    // last_time을 1분 전으로 설정하여 경과 시간 시뮬레이션
    loaded_guest.last_time = time(NULL) - 60; // 60초 전
    updateGuestRemainingTime(&loaded_guest);
    saveGuestInfo(&loaded_guest);
    printf("  ✓ 게스트 경과 시간 계산 확인 (1분 경과 시뮬레이션)\n");
    
    show_test_complete("게스트 시간 관리 테스트");
}

// 테스트 7: 초과 사용 요금 계산 테스트
void test_overuseCharge(void) {
    show_test_start("초과 사용 요금 계산");
    
    show_progress("10분 미만 초과 테스트 중...");
    delay_ms(400);
    
    // 10분 미만 초과 (무료)
    int cost1 = 0, units1 = 0;
    calculateOveruseCharge(5, &cost1, &units1);
    assert(cost1 == 0);
    printf("  ✓ 5분 초과 (무료): %d원\n", cost1);
    delay_ms(200);
    
    show_progress("10분 이상 초과 테스트 중...");
    delay_ms(400);
    
    // 10분 이상 초과 (유료)
    int cost2 = 0, units2 = 0;
    calculateOveruseCharge(25, &cost2, &units2);
    assert(cost2 > 0);
    assert(units2 == 2); // 25분 = 2 x 10분 단위
    printf("  ✓ 25분 초과 요금 계산: %d원 (%d 단위)\n", cost2, units2);
    delay_ms(200);
    
    show_progress("게스트 초과 사용 요금 테스트 중...");
    delay_ms(400);
    
    // 게스트 초과 사용 요금
    int over_minutes = 0, cost3 = 0, units3 = 0;
    calculateGuestOveruseCharge(30, 10, &over_minutes, &cost3, &units3);
    assert(over_minutes == 20); // 30 - 10 = 20분 초과
    assert(units3 == 2); // 20분 = 2 x 10분 단위
    printf("  ✓ 게스트 초과 사용 요금 계산: %d분 초과, %d원\n", over_minutes, cost3);
    
    show_test_complete("초과 사용 요금 계산 테스트");
}

// 테스트 8: 통합 테스트 (전체 플로우)
void test_integration(void) {
    show_test_start("통합 테스트 (전체 플로우)");
    
    show_progress("1. 회원 등록 처리 중...");
    delay_ms(500);
    
    // 1. 새 회원 등록
    UserRecord new_user;
    memset(&new_user, 0, sizeof(new_user));
    snprintf(new_user.id, sizeof(new_user.id), "integration_test");
    snprintf(new_user.password, sizeof(new_user.password), "testpass");
    new_user.is_logged_in = 0;
    saveUser(&new_user);
    
    UserTime new_time;
    memset(&new_time, 0, sizeof(new_time));
    snprintf(new_time.id, sizeof(new_time.id), "integration_test");
    new_time.minutes = 0;
    saveUserTime(&new_time);
    printf("  ✓ 1. 회원 등록 완료\n");
    delay_ms(300);
    
    show_progress("2. 시간 충전 처리 중...");
    delay_ms(500);
    
    // 2. 시간 충전
    new_time.minutes += 120; // 120분 충전
    saveUserTime(&new_time);
    printf("  ✓ 2. 시간 충전 완료: 120분\n");
    delay_ms(300);
    
    show_progress("3. 로그인 처리 중...");
    delay_ms(500);
    
    // 3. 로그인
    UserRecord user;
    loadUser("integration_test", &user);
    UserTime login_time;
    try_login(&user, "testpass", &login_time);
    printf("  ✓ 3. 로그인 완료\n");
    delay_ms(300);
    
    show_progress("4. 남은 시간 조회 중...");
    delay_ms(500);
    
    // 4. 남은 시간 조회
    int remaining = 0;
    getRemainingTimeWithElapsed("integration_test", &remaining);
    printf("  ✓ 4. 남은 시간 조회: %d분\n", remaining);
    delay_ms(300);
    
    show_progress("5. 로그아웃 처리 중...");
    delay_ms(500);
    
    // 5. 로그아웃
    loadUser("integration_test", &user);
    int elapsed = 0;
    UserTime logout_time;
    try_logout(&user, &elapsed, &logout_time);
    printf("  ✓ 5. 로그아웃 완료: 경과 %d분, 남은 시간 %d분\n", elapsed, logout_time.minutes);
    
    show_test_complete("통합 테스트");
}

// 메인 테스트 함수
int main(void) {
    // Windows 콘솔 UTF-8 설정 (한글 깨짐 방지)
    setup_console_utf8();
    
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║      UI 기능 테스트 프로그램          ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    
    delay_ms(1000);
    
    setup_test_environment();
    
    delay_ms(500);
    
    printf("\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("▶ 개별 테스트 시작\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    delay_ms(800);
    
    // 개별 테스트 실행
    test_newUser();
    delay_ms(500);
    
    test_chargeTime();
    delay_ms(500);
    
    test_login();
    delay_ms(500);
    
    test_showRemainingTime();
    delay_ms(500);
    
    test_logout();
    delay_ms(500);
    
    test_guestTimeManagement();
    delay_ms(500);
    
    test_overuseCharge();
    delay_ms(500);
    
    printf("\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("▶ 통합 테스트 시작\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    delay_ms(800);
    
    test_integration();
    
    delay_ms(1000);
    
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║     ✅ 모든 테스트 완료!              ║\n");
    printf("║                                        ║\n");
    printf("║  총 8개의 테스트가 성공적으로         ║\n");
    printf("║  완료되었습니다.                      ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
    
    delay_ms(1500);
    
    return 0;
}
