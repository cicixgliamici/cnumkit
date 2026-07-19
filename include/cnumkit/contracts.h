#ifndef CNUMKIT_CONTRACTS_H
#define CNUMKIT_CONTRACTS_H

#include <assert.h>

/**
 * @file contracts.h
 * @brief Pragmatic Design By Contract (DbC) implementation.
 * 
 * Defines macros for preconditions (CNK_REQUIRES) and postconditions (CNK_ENSURES).
 * These serve as the runtime equivalent of the formal ACSL specifications.
 */

/*
 * Runtime contract checking is opt-in through CNUMKIT_ENABLE_CONTRACTS.
 * Public API functions still validate runtime inputs and return error codes.
 */
#if defined(CNUMKIT_ENABLE_CONTRACTS)

    /* Contract violations are programmer errors, so they terminate instead of returning API errors. */
    #define CNK_REQUIRES(condition) assert((condition) && "Precondition failed: " #condition)
    #define CNK_ENSURES(condition)  assert((condition) && "Postcondition failed: " #condition)
    
#else

    /* Disabled contracts must not evaluate conditions or add runtime overhead. */
    #define CNK_REQUIRES(condition) ((void)0)
    #define CNK_ENSURES(condition)  ((void)0)

#endif

#endif /* CNUMKIT_CONTRACTS_H */
