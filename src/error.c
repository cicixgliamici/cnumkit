#include "cnumkit.h"
#include "cnumkit/error.h"

#include <stdio.h>

/* Cross-platform thread-local storage macro */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#define THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)
#define THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__)
#define THREAD_LOCAL __thread
#else
#define THREAD_LOCAL
#endif

/* Thread-local state for error reporting to ensure thread safety */
static THREAD_LOCAL cnk_error_code last_error_code = CNK_SUCCESS;
static THREAD_LOCAL char last_error_message[256] = "Success";

const char* cnk_error_string(cnk_error_code code) {
    switch (code) {
        case CNK_SUCCESS: return "Success";
        case CNK_ERROR_ALLOCATION: return "Memory allocation failed";
        case CNK_ERROR_INVALID_ARGUMENT: return "Invalid argument provided";
        case CNK_ERROR_DIMENSION_MISMATCH: return "Matrix or vector dimensions do not match";
        case CNK_ERROR_SINGULAR_MATRIX: return "Matrix is singular";
        case CNK_ERROR_MATH: return "Mathematical error";
        default: return "Unknown error";
    }
}

void cnk_set_last_error(cnk_error_code code, const char* message) {
    last_error_code = code;

    /* Copy the message so callers may safely pass temporary storage. */
    const char *source = message ? message : cnk_error_string(code);
    (void)snprintf(last_error_message, sizeof(last_error_message), "%s", source);
}

cnk_error_code cnk_get_last_error(void) {
    return last_error_code;
}

const char* cnk_get_last_error_message(void) {
    return last_error_message;
}
