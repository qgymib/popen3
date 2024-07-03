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
    ASSERT_EQ_INT(popen3(&pip, NULL, NULL, argv, (const char* const*)environ, POPEN3_PIPE, POPEN3_PIPE, POPEN3_NULL), 0);

    ASSERT_EQ_INT(popen3_close(pip), 0);
}

