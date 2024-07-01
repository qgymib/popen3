#include <stdlib.h>
#include <string.h>
#include "cutest.h"
#include "tools/__init__.h"

static void _before_all_test(int argc, char* argv[])
{
    int i;
    const char* opt;
    
    for (i = 0; i < argc; i++)
    {
        opt = "--";
        if (strcmp(argv[i], opt) == 0)
        {
            i++;

            if (i == argc)
            {
                fprintf(stderr, "missing tool name\n");
                exit(EXIT_FAILURE);
            }

            int ret = popen3_test_tool_exec(argv[i], argc - i, argv + 1);
            exit(ret);
        }
    }
}

static cutest_hook_t hook = {
    _before_all_test, /* .before_all_test */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

int main(int argc, char* argv[])
{
    return cutest_run_tests(argc, argv, stdout, &hook);
}
