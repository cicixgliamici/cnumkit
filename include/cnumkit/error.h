#ifndef CNUMKIT_ERROR_H
#define CNUMKIT_ERROR_H

#include "cnumkit_export.h"

/**
 * @brief Represents the various error codes returned by cnumkit operations.
 */
typedef enum {
    CNK_SUCCESS = 0,
    CNK_ERROR_ALLOCATION = -1,
    CNK_ERROR_INVALID_ARGUMENT = -2,
    CNK_ERROR_DIMENSION_MISMATCH = -3,
    CNK_ERROR_SINGULAR_MATRIX = -4,
    CNK_ERROR_MATH = -5,
    CNK_ERROR_UNKNOWN = -99
} cnk_error_code;

/**
 * @brief Returns a static string representation of an error code.
 */
CNUMKIT_EXPORT const char* cnk_error_string(cnk_error_code code);

/**
 * @brief Sets the thread-local last error state.
 * This is primarily used internally by the library, but exposed for completeness.
 * 
 * @param code The error code.
 * @param message Optional detailed message (can be NULL).
 */
CNUMKIT_EXPORT void cnk_set_last_error(cnk_error_code code, const char* message);

/**
 * @brief Gets the error code of the last error that occurred in the current thread.
 * @return The error code.
 */
CNUMKIT_EXPORT cnk_error_code cnk_get_last_error(void);

/**
 * @brief Gets the detailed message of the last error that occurred in the current thread.
 * @return A string containing the error message.
 */
CNUMKIT_EXPORT const char* cnk_get_last_error_message(void);

#endif /* CNUMKIT_ERROR_H */
