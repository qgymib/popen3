#include <errno.h>
#include "test.h"
#include "cutest.h"
#include "popen3.h"

TEST(envp, empty)
{
    popen3_t* pip;
    const char* argv[] = {
        _G.SELF_PATH,
        "--",
        "envp",
        NULL,
    };
    ASSERT_EQ_INT(popen3(&pip, NULL, NULL, argv, NULL, 0, POPEN3_PIPE, 0), 0);

    char buff[1024];
    /*
     * The environment should be empty, so nothing write to stdout.
     * In this case stdout is just closed.
     */
    ASSERT_EQ_INT(popen3_stdout(pip, buff, sizeof(buff)), -EPIPE);

    ASSERT_EQ_INT(popen3_close(pip), 0);
}
