#ifndef POPEN3_TEST_TOOLS_INIT_H
#define POPEN3_TEST_TOOLS_INIT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct popen3_test_tool
{
    /**
     * @brief Name of the tool.
     */
    const char* name;

    /**
     * @brief The entry of the tool.
     */
    int (*entry)(int argc, char* argv[]);
} popen3_test_tool_t;

extern const popen3_test_tool_t popen3_test_tool_echo;
extern const popen3_test_tool_t popen3_test_tool_envp;
extern const popen3_test_tool_t popen3_test_tool_sha256;

int popen3_test_tool_exec(const char* name, int argc, char* argv[]);

#ifdef __cplusplus
}
#endif
#endif
