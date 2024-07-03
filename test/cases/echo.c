#include <string.h>
#include "test.h"
#include "cutest.h"
#include "popen3.h"

#if defined(_WIN32)
#define environ _environ
#endif

extern char** environ;

TEST(echo, echo)
{
    popen3_t* pip;
    const char* argv[] = {
        _G.SELF_PATH,
        "--",
        "echo",
        NULL,
    };
    ASSERT_EQ_INT(popen3(&pip, NULL, NULL, argv, NULL, POPEN3_PIPE, POPEN3_PIPE, POPEN3_NULL), 0);

    const char* data = "hello world";
    const size_t data_sz = strlen(data);
    ASSERT_EQ_INT(popen3_stdin(pip, data, data_sz), data_sz);
    /* Shutdown to close stdin to exit the process, so that the stdout can be fflush to read. */
    ASSERT_EQ_INT(popen3_shutdown_stdin(pip), 0);

    char buff[1024];
    ASSERT_EQ_INT(popen3_stdout(pip, buff, sizeof(buff)), data_sz);
    ASSERT_EQ_INT(memcmp(data, buff, data_sz), 0);

    ASSERT_EQ_INT(popen3_close(pip), 0);
}

