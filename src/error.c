#include "cnumkit.h"
#include "cnumkit/error.h"

/* Cross-platform thread-local storage macro */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#include <threads.h>
#define THREAD_LOCAL thread_local
#elif defined(_MSC_VER)
#define THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__)
#define THREAD_LOCAL __thread
#else
#define THREAD_LOCAL
#endif

static THREAD_LOCAL cnk_error_code last_error_code = CNK_SUCCESS;
static THREAD_LOCAL const char* last_error_message = "Success";

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
    last_error_message = message ? message : cnk_error_string(code);
}

cnk_error_code cnk_get_last_error(void) {
    return last_error_code;
}

const char* cnk_get_last_error_message(void) {
    return last_error_message;
}
