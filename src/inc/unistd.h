/* 
 * unistd.h - fleurer<me.ssword@gmail.com>
 * some macro helpers on making syscall.
 **/

#ifndef UNISTD_H
#define UNISTD_H

#include <signal.h>

#define _SYS0(T0, FN)                           \
    T0 FN(){                                    \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r),                           \
             "=b"(errno)                        \
            :"a"(NR_##FN)                       \
        );                                      \
        return r;                               \
    }

#define _SYS1(T0, FN, T1)                       \
    T0 FN(T1 p1){                               \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r),                           \
             "=b"(errno)                        \
            :"a"(NR_##FN),                      \
             "b"((int)p1)                       \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

#define _SYS2(T0, FN, T1, T2)                   \
    T0 FN(T1 p1, T2 p2){                        \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r),                           \
             "=b"(errno)                        \
            :"a"(NR_##FN),                      \
             "b"((int)p1),                      \
             "c"((int)p2)                       \
        );                                      \
        return r;                               \
    }

#define _SYS3(T0, FN, T1, T2, T3)               \
    T0 FN(T1 p1, T2 p2, T3 p3){                 \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r),                           \
             "=b"(errno)                        \
            :"a"(NR_##FN),                      \
             "b"((int)p1),                      \
             "c"((int)p2),                      \
             "d"((int)p3)                       \
        );                                      \
        return r;                               \
    }

extern int errno;

enum {
    //
    NR_setup, 
    //
    NR_fork,
    NR__exit,
    NR_wait,
    NR_waitpid,
    //
    NR_nice,
    NR_getpid,
    NR_setuid,
    NR_chdir,
    NR_exec,
    // on signal
    NR_kill,
    NR_signal,
    NR_sigaction,
    NR_sigreturn,
    // on fs
    NR_access,
    NR_chown,
    NR_open,
    NR_close,
    NR_creat,
    NR_link,
    NR_unlink,
    NR_mknod,
    NR_read,
    NR_write,
    NR_truncate,
    NR_fcntl,
    NR_mount,
    NR_umount,
    //
    NR_debug
};

int nosys       (struct trap *);
int sys_setup   (struct trap *);
int sys_access  (struct trap *);
int sys_fork    (struct trap *);
int sys_nice    (struct trap *);
int sys_debug   (struct trap *);
int sys_getpid  (struct trap *);
int sys_open    (struct trap *);
int sys_write   (struct trap *);
int sys_exec    (struct trap *);
int sys__exit   (struct trap *);
int sys_kill    (struct trap *);
int sys_signal  (struct trap *);
int sys_wait    (struct trap *);
int sys_waitpid (struct trap *);
int sys_sigaction (struct trap *);
int sys_sigreturn (struct trap *);

static inline _SYS0(int, debug);
static inline _SYS1(int, close, int);
static inline _SYS0(int, setup);
static inline _SYS0(int, fork); 
static inline _SYS1(int, nice, int);
static inline _SYS1(int, chdir, char*);
static inline _SYS1(int, setuid, int);
static inline _SYS0(int, getpid);
static inline _SYS2(int, access, char*, int);
static inline _SYS3(int, write, int, char*, int);
static inline _SYS3(int, open, char*, int, int);
static inline _SYS2(int, exec, char*, char**);
static inline _SYS1(int, _exit, int);
static inline _SYS2(int, kill, int, int);
static inline _SYS2(int, signal, int, uint);
static inline _SYS3(int, sigaction, int, struct sigaction*, struct sigaction*);
static inline _SYS0(int, sigreturn);
static inline _SYS3(int, waitpid, int, int*, int);
static inline _SYS1(int, wait, int*);

#endif
