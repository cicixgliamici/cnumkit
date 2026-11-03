#ifndef CNUMKIT_CONTRACTS_H
#define CNUMKIT_CONTRACTS_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @file contracts.h
 * @brief Optional fail-fast checks for internal implementation invariants.
 *
 * Public argument errors remain recoverable in either mode. These checks are
 * not a runtime verifier for every ACSL specification in the public headers.
 */

/*
 * Enable with CNUMKIT_ENABLE_CONTRACTS=1; undefined or zero disables checks.
 * Enabled checks evaluate once and abort on failure, independently of NDEBUG.
 * Conditions must be free of side effects because disabled checks skip them.
 */
#if defined(CNUMKIT_ENABLE_CONTRACTS) && CNUMKIT_ENABLE_CONTRACTS

/* A private inline helper keeps direct C/C++ header use independent of linkage. */
static inline void cnk_contract_fail(const char *kind, const char *condition,
    const char *file, int line) {
    fprintf(stderr, "cnumkit %s failed: %s (%s:%d)\n", kind, condition, file, line);
    /* Unlike assert, an explicitly enabled contract must also run in Release. */
    abort();
}

    #define CNK_REQUIRES(condition) \
        do { \
            if (!(condition)) { \
                cnk_contract_fail("precondition", #condition, __FILE__, __LINE__); \
            } \
        } while (0)
    #define CNK_ENSURES(condition) \
        do { \
            if (!(condition)) { \
                cnk_contract_fail("postcondition", #condition, __FILE__, __LINE__); \
            } \
        } while (0)
    
#else

    /* Disabled contracts must not evaluate conditions or add runtime overhead. */
    #define CNK_REQUIRES(condition) ((void)0)
    #define CNK_ENSURES(condition)  ((void)0)

#endif

#endif /* CNUMKIT_CONTRACTS_H */
