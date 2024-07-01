#ifndef POPEN3_TEST_H
#define POPEN3_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct popen3_test_ctx
{
    /**
     * @brief Path to self.
     */
    const char* SELF_PATH;
} popen3_test_ctx_t;

/**
 * @brief Global test runtime.
 */
extern popen3_test_ctx_t _G;

#ifdef __cplusplus
}
#endif
#endif

