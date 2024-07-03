#ifndef POPEN3_TEST_H
#define POPEN3_TEST_H

#include "cutest.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct popen3_test_ctx
{
    /**
     * @brief Path to self.
     */
    char* SELF_PATH;
} popen3_test_ctx_t;

/**
 * @brief Global test runtime.
 */
extern popen3_test_ctx_t _G;

/**
 * @brief Test hook.
 */
extern cutest_hook_t g_hook;

#ifdef __cplusplus
}
#endif
#endif

