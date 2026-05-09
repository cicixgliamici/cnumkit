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
 * By default, we enable runtime contract checking in debug builds,
 * or if CNUMKIT_ENABLE_CONTRACTS is explicitly defined.
 * In production/release, they are compiled away for zero overhead.
 */
#if defined(CNUMKIT_ENABLE_CONTRACTS) || !defined(NDEBUG)

    /* We use assert, but we could hook this into our error handling system! */
    #define CNK_REQUIRES(condition) assert((condition) && "Precondition failed: " #condition)
    #define CNK_ENSURES(condition)  assert((condition) && "Postcondition failed: " #condition)
    
#else

    /* Zero overhead in Release */
    #define CNK_REQUIRES(condition) ((void)0)
    #define CNK_ENSURES(condition)  ((void)0)

#endif

#endif /* CNUMKIT_CONTRACTS_H */
