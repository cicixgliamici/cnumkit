#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
 * CNK_TEST_EPSILON defines the default tolerance for EXPECT_ALMOST_EQ.
 * Test suites can override this before including test_framework.h
 * if they require a different numerical tolerance.
 */
#ifndef CNK_TEST_EPSILON
#define CNK_TEST_EPSILON 1e-9
#endif

#ifndef CNK_TEST_RELATIVE_EPSILON
#define CNK_TEST_RELATIVE_EPSILON CNK_TEST_EPSILON
#endif

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

static int cnk_test_almost_equal(double actual, double expected) {
    if (!isfinite(actual) || !isfinite(expected)) {
        return actual == expected;
    }

    double difference = fabs(actual - expected);
    double scale = fmax(fabs(actual), fabs(expected));
    return difference <= CNK_TEST_EPSILON + CNK_TEST_RELATIVE_EPSILON * scale;
}

#define EXPECT_TRUE(cond) \
    do { \
        tests_run++; \
        if (cond) { tests_passed++; } \
        else { \
            printf("[FAILED] %s:%d: Expected " #cond " to be true\n", __FILE__, __LINE__); \
            tests_failed++; \
        } \
    } while (0)

#define EXPECT_FALSE(cond) EXPECT_TRUE(!(cond))

#define EXPECT_ALMOST_EQ(a, b) EXPECT_TRUE(cnk_test_almost_equal((a), (b)))

#define EXPECT_NOT_NULL(ptr) EXPECT_TRUE((ptr) != NULL)
#define EXPECT_NULL(ptr) EXPECT_TRUE((ptr) == NULL)

#define TEST_SUITE(name) \
    int main(void) { \
        printf("========================================\n"); \
        printf("Running test suite: %s\n", name); \
        printf("========================================\n"); \
        tests_run = 0; tests_passed = 0; tests_failed = 0;

#define RUN_TEST(test_func) \
    do { \
        printf("  Running %s... ", #test_func); \
        int initial_failed = tests_failed; \
        test_func(); \
        if (tests_failed == initial_failed) { \
            printf("OK\n"); \
        } \
    } while(0)

#define TEST_SUITE_END() \
        printf("\nTest Summary:\n"); \
        printf("  Total:  %d\n", tests_run); \
        printf("  Passed: %d\n", tests_passed); \
        printf("  Failed: %d\n", tests_failed); \
        return (tests_failed == 0) ? 0 : 1; \
    }

#endif /* TEST_FRAMEWORK_H */
