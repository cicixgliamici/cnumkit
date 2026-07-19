#ifndef CNUMKIT_ERROR_H
#define CNUMKIT_ERROR_H

#include "cnumkit_export.h"

/**
 * @file error.h
 * @brief Thread-local error reporting for public cnumkit operations.
 *
 * Successful computational, allocation, access, mutation, print, and free
 * operations set the thread-local state to CNK_SUCCESS. On failure, functions
 * set a specific error code and message. Error inspection functions do not
 * modify the state. Output parameters remain unchanged when an operation fails.
 */

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
 * @param code Error code to describe.
 * @return A pointer to immutable static storage. The caller must not free it.
 * @note This function does not modify the thread-local error state.
 */
CNUMKIT_EXPORT const char* cnk_error_string(cnk_error_code code);

/**
 * @brief Sets the thread-local last error state.
 * This is primarily used internally by the library, but exposed for completeness.
 * 
 * @param code The error code.
 * @param message Optional message. The function copies and may truncate it to
 *        fit its internal thread-local buffer.
 */
CNUMKIT_EXPORT void cnk_set_last_error(cnk_error_code code, const char* message);

/**
 * @brief Gets the error code of the last error that occurred in the current thread.
 * @return The error code.
 * @note This function does not modify the thread-local error state.
 */
CNUMKIT_EXPORT cnk_error_code cnk_get_last_error(void);

/**
 * @brief Gets the detailed message of the last error that occurred in the current thread.
 * @return A borrowed string that must not be modified or freed. It remains valid
 *         until the next cnk_set_last_error() call in the same thread.
 * @note This function does not modify the thread-local error state.
 */
CNUMKIT_EXPORT const char* cnk_get_last_error_message(void);

#endif /* CNUMKIT_ERROR_H */
