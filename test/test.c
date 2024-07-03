#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "test.h"
#include "tools/__init__.h"

popen3_test_ctx_t _G;

static char* _popen3_test_exepath(void)
{
    size_t buff_sz = 4096;
    char* buff = malloc(buff_sz);
    if (readlink("/proc/self/exe", buff, buff_sz) < 0)
    {
        abort();
    }

    return buff;
}

static void _before_all_test(int argc, char* argv[])
{
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--") == 0)
        {
            i++;

            if (i == argc)
            {
                fprintf(stderr, "missing tool name\n");
                exit(EXIT_FAILURE);
            }

            int ret = popen3_test_tool_exec(argv[i], argc - i, argv + i);
            exit(ret);
        }
    }

    _G.SELF_PATH = _popen3_test_exepath();
}

static void _after_all_test(void)
{
    if (_G.SELF_PATH != NULL)
    {
        free(_G.SELF_PATH);
        _G.SELF_PATH = NULL;
    }
}

cutest_hook_t g_hook = {
    _before_all_test, /* .before_all_test */
    _after_all_test,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
