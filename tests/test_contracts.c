/* Exercise both NDEBUG states even when the outer build is Release. */
#if defined(CNUMKIT_TEST_WITHOUT_NDEBUG)
#undef NDEBUG
#elif defined(CNUMKIT_TEST_WITH_NDEBUG) && !defined(NDEBUG)
#define NDEBUG 1
#endif

#include "cnumkit/contracts.h"

#include <string.h>

#if CNUMKIT_ENABLE_CONTRACTS
#if defined(_WIN32)
#include <windows.h>
#else
#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

static void violate_contract(const char *kind) {
    if (strcmp(kind, "requires") == 0) {
        CNK_REQUIRES(0);
    } else if (strcmp(kind, "ensures") == 0) {
        CNK_ENSURES(0);
    }
    /* Returning, an unknown case, or a disabled check must fail the death test. */
    _Exit(71);
}

#if defined(_WIN32)
/* Classify only the expected abort and the deliberate ordinary-exit control. */
static int child_aborted(const char *kind) {
    char executable[4096];
    char command[8192];
    DWORD length = GetModuleFileNameA(NULL, executable, (DWORD)sizeof(executable));
    if (length == 0 || length >= sizeof(executable)) {
        return -1;
    }
    int written = snprintf(command, sizeof(command), "\"%s\" --child %s", executable, kind);
    if (written < 0 || (size_t)written >= sizeof(command)) {
        return -1;
    }
    STARTUPINFOA startup = {0};
    PROCESS_INFORMATION process = {0};
    startup.cb = (DWORD)sizeof(startup);
    /* Keep expected child failures non-interactive on Windows CI and desktops. */
    if (!CreateProcessA(executable, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
        NULL, NULL, &startup, &process)) {
        return -1;
    }
    DWORD exit_code = 0;
    DWORD wait_result = WaitForSingleObject(process.hProcess, 5000);
    int aborted = -1;
    if (wait_result == WAIT_OBJECT_0 && GetExitCodeProcess(process.hProcess, &exit_code)) {
        aborted = exit_code == 3 ? 1 : (exit_code == 71 ? 0 : -1);
    }
    if (wait_result != WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess, 74);
        WaitForSingleObject(process.hProcess, 5000);
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    /* With CRT report-fault behavior disabled below, abort exits with code 3. */
    return aborted;
}
#else
static int child_aborted(const char *kind) {
    pid_t child = fork();
    if (child == -1) {
        return -1;
    }
    if (child == 0) {
        /* Expected aborts must not leave large core files in a checkout. */
        const struct rlimit limit = {0, 0};
        if (setrlimit(RLIMIT_CORE, &limit) != 0) {
            _Exit(73);
        }
        violate_contract(kind);
    }
    int status = 0;
    pid_t waited;
    do {
        waited = waitpid(child, &status, 0);
    } while (waited == -1 && errno == EINTR);
    if (waited != child) {
        return -1;
    }
    /* Reject unrelated signals and setup errors instead of accepting any crash. */
    if (WIFSIGNALED(status)) {
        return WTERMSIG(status) == SIGABRT ? 1 : -1;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 71 ? 0 : -1;
}
#endif
#endif

static int check_evaluation_count(void) {
    int evaluations = 0;
    /* Side effects are used only here to test evaluation, never as real checks. */
    CNK_REQUIRES(++evaluations == 1);
    CNK_ENSURES(++evaluations == 2);
#if CNUMKIT_ENABLE_CONTRACTS
    return evaluations == 2;
#else
    CNK_REQUIRES(0);
    CNK_ENSURES(0);
    return evaluations == 0;
#endif
}

int main(int argc, char **argv) {
#if CNUMKIT_ENABLE_CONTRACTS && defined(_WIN32)
    if (argc == 3 && strcmp(argv[1], "--child") == 0) {
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#if defined(_MSC_VER)
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
        violate_contract(argv[2]);
    }
#else
    (void)argc;
    (void)argv;
#endif
    if (!check_evaluation_count()) {
        fprintf(stderr, "Contract condition evaluation count is incorrect\n");
        return 1;
    }
#if CNUMKIT_ENABLE_CONTRACTS
    if (child_aborted("requires") != 1 || child_aborted("ensures") != 1
        || child_aborted("ordinary-exit") != 0) {
        fprintf(stderr, "Contract child did not have the expected abort outcome\n");
        return 1;
    }
#endif
    return 0;
}
