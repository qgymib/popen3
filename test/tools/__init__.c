#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "__init__.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static const popen3_test_tool_t s_popen3_test_tool_help;

static const popen3_test_tool_t* s_tools[] = {
    &popen3_test_tool_echo,
    &s_popen3_test_tool_help,
};

int popen3_test_tool_exec(const char* name, int argc, char* argv[])
{
    size_t i;
    for (i = 0; i < ARRAY_SIZE(s_tools); i++)
    {
        if (strcmp(name, s_tools[i]->name) == 0)
        {
            return s_tools[i]->entry(argc, argv);
        }
    }
    fprintf(stderr, "%s: command not found\n", name);
    return EXIT_FAILURE;
}

static int _popen3_test_tool_help_entry(int argc, char* argv[])
{
    (void)argc; (void)argv;

    printf("Builtin tools:\n");

    size_t i;
    for (i = 0; i < ARRAY_SIZE(s_tools); i++)
    {
        printf("- %s\n", s_tools[i]->name);
    }

    return 0;
}

static const popen3_test_tool_t s_popen3_test_tool_help = {
    "help", _popen3_test_tool_help_entry,
};
