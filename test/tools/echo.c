#include <stdio.h>
#include "__init__.h"

static int _popen3_test_tool_echo(int argc, char* argv[])
{
    (void)argc; (void)argv;

    int c;
    while ((c = fgetc(stdin)) != EOF)
    {
        fputc(c, stdout);
    }

    return 0;
}

const popen3_test_tool_t popen3_test_tool_echo = {
    "echo", _popen3_test_tool_echo,
};
