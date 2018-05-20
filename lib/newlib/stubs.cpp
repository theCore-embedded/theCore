/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Newlib stubs implementation.
//! \details Not all newlib functionality is allowed (by default) in theCore.
//! Examples include forking or using memory allocations.

#include <common/execution.hpp>
#include <common/console.hpp>

#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <errno.h>

#undef errno
extern int errno;

extern "C"
void _exit(int status)
{
    (void)status;
    ecl::abort();
}

extern "C"
int _close(int file)
{
    (void)file;
    errno = EBADF;
    return -1;
}

int _isatty(int file)
{
    switch (file)
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        errno = EBADF;
        return 0;
    }
}

extern "C"
char *__env[1];

char *__env[1] = { 0 };

extern "C"
char **environ;

char **environ = __env;

extern "C"
int _execve(const char *name, char * const *argv, char * const * env)
{
    (void)name;
    (void)argv;
    (void)env;
    errno = ENOMEM;
    return -1;
}

extern "C"
int _fork(void)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int _fstat(int file, struct stat *st)
{
    (void)st;
    (void)file;
    errno = EBADF;
    return -1;
}

extern "C"
int _getpid(void)
{
    return 1;
}

extern "C"
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

extern "C"
int _link(const char *old, const char *nw)
{
    (void)old;
    (void)nw;
    errno = EBADF;
    return -1;
}

extern "C"
off_t _lseek(int file, off_t oft, int dir)
{
    (void)file;
    (void)oft;
    (void)dir;
    errno = EBADF;
    return -1;
}

extern "C"
int _read(int file, void *ptr, size_t len)
{
    (void)file;
    (void)ptr;
    (void)len;
    errno = EBADF;
    return -1;
}

extern uint8_t __HeapBase;
extern uint8_t __HeapLimit;
static uint8_t *heap_current = &__HeapBase;
static uint8_t *heap_end = &__HeapLimit;

extern "C"
void *_sbrk(ptrdiff_t incr)
{
    if (heap_current + incr > heap_end) {
        ecl::abort(); // TODO: better handling
        errno = ENOMEM;
        return nullptr;
    }

    heap_current += incr;
    return heap_current;
}

extern "C"
int _stat(const char *__restrict file, struct stat *__restrict st)
{
    (void)file;
    (void)st;
    errno = EBADF;
    return -1;
}

extern "C"
clock_t _times(struct tms *buf)
{
    (void)buf;
    return -1;
}

extern "C"
int _unlink(const char *name)
{
    (void)name;
    errno = EBADF;
    return -1;
}

extern "C"
int _wait(int *status)
{
    (void)status;
    errno = ECHILD;
    return -1;
}

extern "C"
int _write(int file, const void *ptr, size_t sz)
{
    if (file != STDOUT_FILENO || file != STDERR_FILENO) {
        errno = EBADF;
        return -1;
    }

    for (size_t i = 0; i < sz; ++i) {
        ecl::bypass_putc(static_cast<const char*>(ptr)[i]);
    }

    return 0;
}
