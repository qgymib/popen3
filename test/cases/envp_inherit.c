#include "test.h"
#include "cutest.h"
#include "popen3.h"

TEST(envp, inherit)
{
    popen3_t* pip;
    const char* argv[] = {
        _G.SELF_PATH,
        "--",
        "envp",
        NULL,
    };
    const char* envp[] = {
        "a=b",
        "c=d",
        NULL,
    };
    ASSERT_EQ_INT(popen3(&pip, NULL, NULL, argv, envp, 0, POPEN3_PIPE, 0), 0);

    char buff[1024];
    ASSERT_EQ_INT(popen3_stdout(pip, buff, sizeof(buff)), 8);

    ASSERT_EQ_INT(popen3_close(pip), 0);
}
