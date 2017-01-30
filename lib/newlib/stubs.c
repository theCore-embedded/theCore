//! \file
//! \brief Newlib stubs implementation.
//! \details Not all newlib functionality is allowed (by default) in theCore.
//! Examples include forking or using memory allocations.

#include <common/execution.h>

#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <errno.h>

#undef errno
extern int errno;

void _exit(int status)
{
    (void)status;
    ecl_abort();
}

int _close(int file)
{
    (void)file;
    errno = EBADF;
    return -1;
}

char *__env[1] = { 0 };

char **environ = __env;

int _execve(const char *name, char * const *argv, char * const * env)
{
    (void)name;
    (void)argv;
    (void)env;
    errno = ENOMEM;
    return -1;
}

int _fork(void)
{
    errno = ENOSYS;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void)st;
    (void)file;
    errno = EBADF;
    return -1;
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _link(const char *old, const char *nw)
{
    (void)old;
    (void)nw;
    errno = EBADF;
    return -1;
}

off_t _lseek(int file, off_t oft, int dir)
{
    (void)file;
    (void)oft;
    (void)dir;
    errno = EBADF;
    return -1;
}

int _read(int file, void *ptr, size_t len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = EBADF;
    return -1;
}

void *_sbrk(ptrdiff_t incr)
{
    (void)incr;
    ecl_abort(); // Dynamic allocation is not allowed by default.
}

int _stat(const char *__restrict file, struct stat *__restrict st)
{
    (void)file;
    (void)st;
    errno = EBADF;
    return -1;
}

clock_t _times(struct tms *buf)
{
    (void)buf;
    return -1;
}

int _unlink(const char *name)
{
    (void)name;
    errno = EBADF;
    return -1;
}

int _wait(int *status)
{
    (void)status;
    errno = ECHILD;
    return -1;
}

int _write(int file, const void *ptr, size_t sz)
{
    (void)file;
    (void)ptr;
    (void)sz;
    errno = EBADF;
    return -1;
}
