#include <stdlib.h>
#include <string.h>
#include "cutest.h"
#include "test.h"
#include "tools/__init__.h"

static void _before_all_test(int argc, char* argv[])
{
    int i;
    const char* opt;
    size_t opt_sz;

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

        opt = "--self-path";
        opt_sz = strlen(opt);
        if (strncmp(argv[i], opt, opt_sz) == 0)
        {
            if (argv[i][opt_sz] == '=')
            {
                _G.SELF_PATH = argv[i] + opt_sz + 1;
            }
            else if (i == argc - 1)
            {
                fprintf(stderr, "missing argument to `--self-path`\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                i++;
                _G.SELF_PATH = argv[i];
            }
            continue;
        }
    }

    if (_G.SELF_PATH == NULL)
    {
        fprintf(stderr, "missing argument `--self-path`\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("--self-path=%s\n", _G.SELF_PATH);
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
