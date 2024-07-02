#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "__init__.h"

#if defined(_WIN32)
#define environ _environ
#endif

extern char** environ;

static const char* s_help =
"envp - print environments.\n"
"-h, --help\n"
"    Show this help and exit.\n";

static void _popen3_test_tool_envp_print(char** env)
{
    for (; *env != NULL; env++)
    {
        printf("%s\n", *env);
    }
}

static int _popen3_test_tool_envp(int argc, char* argv[])
{
    int i;

    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("%s", s_help);
            return 0;
        }
    }

    _popen3_test_tool_envp_print(environ);
    return 0;
}

const popen3_test_tool_t popen3_test_tool_envp = {
    "envp", _popen3_test_tool_envp,
};
