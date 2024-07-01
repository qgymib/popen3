#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "popen3.h"

#if defined(_WIN32)
#else

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

typedef pid_t popen3_pid_t;
#define POPEN3_PID_INVALID -1

typedef int popen3_pip_t;
#define POPEN3_PIP_INVALID -1

#if defined(_AIX) || \
    defined(__APPLE__) || \
    defined(__DragonFly__) || \
    defined(__FreeBSD__) || \
    defined(__FreeBSD_kernel__) || \
    defined(__linux__) || \
    defined(__OpenBSD__) || \
    defined(__NetBSD__)

#define INFRA_USE_IOCTL

#else

static int ev__fcntl_getfd_unix(int fd)
{
    int flags;

    do
    {
        flags = fcntl(fd, F_GETFD);
    } while (flags == -1 && errno == EINTR);

    return flags;
}

static int ev__fcntl_getfl_unix(int fd)
{
    int mode;
    do
    {
        mode = fcntl(fd, F_GETFL);
    } while (mode == -1 && errno == EINTR);
    return mode;
}

#endif

static int _popen3_pip_cloexec(int fd, int set)
{
#if defined(INFRA_USE_IOCTL)
    int r;

    do
    {
        r = ioctl(fd, set ? FIOCLEX : FIONCLEX);
    } while (r == -1 && errno == EINTR);

    if (r)
    {
        return -errno;
    }

    return 0;

#else

    int flags;

    int r = ev__fcntl_getfd_unix(fd);
    if (r == -1)
    {
        return -errno;
    }

    /* Bail out now if already set/clear. */
    if (!!(r & FD_CLOEXEC) == !!set)
    {
        return 0;
    }

    if (set)
    {
        flags = r | FD_CLOEXEC;
    }
    else
    {
        flags = r & ~FD_CLOEXEC;
    }

    do
    {
        r = fcntl(fd, F_SETFD, flags);
    } while (r == -1 && errno == EINTR);

    if (r)
    {
        return -errno;
    }

    return 0;

#endif
}

static int _popen3_pip_nonblock(int fd, int set)
{
#if defined(INFRA_USE_IOCTL)

    int r;

    do
    {
        r = ioctl(fd, FIONBIO, &set);
    } while (r == -1 && errno == EINTR);

    if (r)
    {
        return -errno;
    }

    return 0;

#else

    int flags;

    int r = ev__fcntl_getfl_unix(fd);
    if (r == -1)
    {
        return -errno;
    }

    /* Bail out now if already set/clear. */
    if (!!(r & O_NONBLOCK) == !!set)
    {
        return 0;
    }

    if (set)
    {
        flags = r | O_NONBLOCK;
    }
    else
    {
        flags = r & ~O_NONBLOCK;
    }

    do
    {
        r = fcntl(fd, F_SETFL, flags);
    } while (r == -1 && errno == EINTR);

    if (r)
    {
        return -errno;
    }

    return 0;

#endif
}

static int _popen3_waitpid(popen3_pid_t pid)
{
    int wstatus = 0;
    waitpid(pid, &wstatus, 0);

    if (WIFEXITED(wstatus))
    {
        return WEXITSTATUS(wstatus);
    }

    return -1;
}

static int _popen3_pip_open(popen3_pip_t pip[2])
{
    if (pipe(pip) == 0)
    {
        return 0;
    }
    return -errno;
}

static void _popen3_pip_close(popen3_pip_t pip)
{
    close(pip);
}

static int _popen3_pip_write(popen3_pip_t pip, const void* data, size_t size)
{
    ssize_t write_sz = write(pip, data, size);
    if (write_sz >= 0)
    {
        return write_sz;
    }

    int errcode = errno;
    if (errcode == EAGAIN || errcode == EWOULDBLOCK)
    {
        return 0;
    }
    return -errcode;
}

static int _popen3_pip_read(popen3_pip_t pip, void* buff, size_t size)
{
    ssize_t read_sz = read(pip, buff, size);
    if (read_sz > 0)
    {
        return read_sz;
    }

    if (read_sz == 0)
    {
        return -EPIPE;
    }

    int errcode = errno;
    if (errcode == EAGAIN || errcode == EWOULDBLOCK)
    {
        return 0;
    }
    return -errcode;
}

static void _popen3_execute_child(const char* cmd, popen3_pid_t pstatus[2],
    popen3_pip_t pstdin, popen3_pip_t pstdout, popen3_pip_t pstderr)
{
    _popen3_pip_close(pstatus[0]);
    pstatus[0] = POPEN3_PIP_INVALID;

    if (pstdin != POPEN3_PIP_INVALID)
   {
        dup2(pstdin, STDIN_FILENO);
    }
    if (pstdout != POPEN3_PIP_INVALID)
    {
        dup2(pstdout, STDOUT_FILENO);
    }
    if (pstderr != POPEN3_PIP_INVALID)
    {
        dup2(pstderr, STDERR_FILENO);
    }

    execl("/bin/sh", "sh", "-c", cmd, NULL);

    ssize_t n;
    int errcode = -errno;
    do
    {
        n = write(pstatus[1], &errcode, sizeof(errcode));
    } while (n == -1 && errno == EINTR); 
}

static int _popen3_execute(popen3_pid_t* pid, const char* cmd,
    popen3_pip_t pstdin, popen3_pip_t pstdout, popen3_pip_t pstderr)
{
    int ret = 0;
    popen3_pip_t pip_status[2];
    if ((ret = _popen3_pip_open(pip_status)) != 0)
    {
        return ret;
    }

    if ((ret = _popen3_pip_cloexec(pip_status[0], 1)) != 0)
    {
        goto error_close_status;
    }

    if ((*pid = fork()) < 0)
    {/* fork failed */
        return -errno;
    }

    if (*pid == 0)
    {
        _popen3_execute_child(cmd, pip_status, pstdin, pstdout, pstderr);
    }

    _popen3_pip_close(pip_status[1]);
    pip_status[1] = POPEN3_PIP_INVALID;

    ssize_t read_sz = 0;
    int errcode = 0;

    do
    {
        read_sz = read(pip_status[0], &errcode, sizeof(errcode));
    } while (read_sz == -1 && errno == EINTR);

    if (read_sz > 0)
    {
        waitpid(*pid, NULL, 0);
        ret = errcode;
        goto error_close_status;
    }
    if (read_sz < 0)
    {/* unknown error. */
        abort();
    }
    
    _popen3_pip_close(pip_status[0]);
    pip_status[0] = POPEN3_PIP_INVALID;

    return 0;
    
error_close_status:
    _popen3_pip_close(pip_status[0]);
    pip_status[0] = POPEN3_PIP_INVALID;
    _popen3_pip_close(pip_status[1]);
    pip_status[1] = POPEN3_PIP_INVALID;
    return ret;
}

#endif

struct popen3_s
{
    popen3_pid_t child_pid;
    popen3_pip_t child_stdin;
    popen3_pip_t child_stdout;
    popen3_pip_t child_stderr;
};

typedef struct popen3_open_helper
{
    popen3_pip_t pipe_stdin[2];
    popen3_pip_t pipe_stdout[2];
    popen3_pip_t pipe_stderr[2];
} popen3_open_helper_t;

static int _popen3_setup_pipe(popen3_open_helper_t* helper, const char* type)
{
    int ret = 0;

    if (strchr(type, 'i') != NULL)
    {
        if ((ret = _popen3_pip_open(helper->pipe_stdin)) != 0)
        {
            goto finish;
        }
        if ((ret = _popen3_pip_cloexec(helper->pipe_stdin[1], 1)) != 0)
        {
            goto error_close_stdin;
        }
        if ((ret = _popen3_pip_nonblock(helper->pipe_stdin[1], 1)) != 0)
        {
            goto error_close_stdin;
        }
    }
    else
    {
        helper->pipe_stdin[0] = POPEN3_PIP_INVALID;
        helper->pipe_stdin[1] = POPEN3_PIP_INVALID;
    }

    if (strchr(type, 'o') != NULL)
    {
        if ((ret = _popen3_pip_open(helper->pipe_stdout)) != 0)
        {
            goto error_close_stdin;
        }
        if ((ret = _popen3_pip_cloexec(helper->pipe_stdout[0], 1)) != 0)
        {
            goto error_close_stdout;
        }
        if ((ret = _popen3_pip_nonblock(helper->pipe_stdout[0], 1)) != 0)
        {
            goto error_close_stdout;
        }
    }
    else
    {
        helper->pipe_stdout[0] = POPEN3_PIP_INVALID;
        helper->pipe_stdout[1] = POPEN3_PIP_INVALID;
    }

    if (strchr(type, 'e') != NULL)
    {
        if ((ret = _popen3_pip_open(helper->pipe_stderr)) != 0)
        {
            goto error_close_stdout;
        }
        if ((ret = _popen3_pip_cloexec(helper->pipe_stderr[0], 1)) != 0)
        {
            goto error_close_stderr;
        }
        if ((ret = _popen3_pip_nonblock(helper->pipe_stderr[0], 1)) != 0)
        {
            goto error_close_stderr;
        }
    }
    else
    {
        helper->pipe_stderr[0] = POPEN3_PIP_INVALID;
        helper->pipe_stderr[1] = POPEN3_PIP_INVALID;
    }

    goto finish;

error_close_stderr:
    _popen3_pip_close(helper->pipe_stderr[0]);
    helper->pipe_stderr[0] = POPEN3_PIP_INVALID;
    _popen3_pip_close(helper->pipe_stderr[1]);
    helper->pipe_stderr[1] = POPEN3_PIP_INVALID;
error_close_stdout:
    _popen3_pip_close(helper->pipe_stdout[0]);
    helper->pipe_stdout[0] = POPEN3_PIP_INVALID;
    _popen3_pip_close(helper->pipe_stdout[1]);
    helper->pipe_stdout[1] = POPEN3_PIP_INVALID;
error_close_stdin:
    _popen3_pip_close(helper->pipe_stdin[0]);
    helper->pipe_stdin[0] = POPEN3_PIP_INVALID;
    _popen3_pip_close(helper->pipe_stdin[1]);
    helper->pipe_stdin[1] = POPEN3_PIP_INVALID;
finish:
    return ret;
}

int popen3(popen3_t** pip, const char* cmd, const char* type)
{
    int ret = 0;

    if ((*pip = malloc(sizeof(popen3_t))) == NULL)
    {
        return -ENOMEM;
    }
    (*pip)->child_pid = POPEN3_PID_INVALID;
    (*pip)->child_stdin = POPEN3_PIP_INVALID;
    (*pip)->child_stdout = POPEN3_PIP_INVALID;
    (*pip)->child_stderr = POPEN3_PIP_INVALID;

    popen3_open_helper_t helper;
    if ((ret = _popen3_setup_pipe(&helper, type)) != 0)
    {
        free(pip);
        return ret;
    }

    ret = _popen3_execute(&(*pip)->child_pid, cmd, helper.pipe_stdin[0],
                          helper.pipe_stdout[1], helper.pipe_stderr[1]);

    _popen3_pip_close(helper.pipe_stdin[0]);
    helper.pipe_stdin[0] = POPEN3_PIP_INVALID;
    _popen3_pip_close(helper.pipe_stdout[1]);
    helper.pipe_stdout[1] = POPEN3_PIP_INVALID;
    _popen3_pip_close(helper.pipe_stderr[1]);
    helper.pipe_stderr[1] = POPEN3_PIP_INVALID;

    (*pip)->child_stdin = helper.pipe_stdin[1];
    (*pip)->child_stdout = helper.pipe_stdout[0];
    (*pip)->child_stderr = helper.pipe_stderr[1];

    if (ret != 0)
    {
        popen3_close(*pip);
    }

    return ret;
}

int popen3_close(popen3_t* pip)
{
    popen3_shutdown_stdin(pip);
    popen3_shutdown_stdout(pip);
    popen3_shutdown_stderr(pip);

    int ret = -EALREADY;
    if (pip->child_pid != POPEN3_PID_INVALID)
    {
        ret = _popen3_waitpid(pip->child_pid);
        pip->child_pid = POPEN3_PID_INVALID;
    }

    free(pip);
    return ret;
}

int popen3_shutdown_stdin(popen3_t* pip)
{
    if (pip->child_stdin == POPEN3_PIP_INVALID)
    {
        return -EALREADY;
    }

    _popen3_pip_close(pip->child_stdin);
    pip->child_stdin = POPEN3_PIP_INVALID;
    return 0;
}

int popen3_shutdown_stdout(popen3_t* pip)
{
    if (pip->child_stdout == POPEN3_PIP_INVALID)
    {
        return -EALREADY;
    }

    _popen3_pip_close(pip->child_stdout);
    pip->child_stdout = POPEN3_PIP_INVALID;
    return 0;
}

int popen3_shutdown_stderr(popen3_t* pip)
{
    if (pip->child_stderr == POPEN3_PIP_INVALID)
    {
        return -EALREADY;
    }

    _popen3_pip_close(pip->child_stderr);
    pip->child_stderr = POPEN3_PIP_INVALID;
    return 0;
}

int popen3_stdin(popen3_t* pip, const void* data, size_t size)
{
    return _popen3_pip_write(pip->child_stdin, data, size);
}

int popen3_stdout(popen3_t* pip, void* buff, size_t size)
{
    return _popen3_pip_read(pip->child_stdout, buff, size);
}

int popen3_stderr(popen3_t* pip, void* buff, size_t size)
{
    return _popen3_pip_read(pip->child_stderr, buff, size);
}
