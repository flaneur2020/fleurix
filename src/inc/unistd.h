/* 
 * unistd.h - fleurer<me.ssword@gmail.com>
 * some macro helpers on making syscall.
 **/

#ifndef UNISTD_H
#define UNISTD_H

#include <signal.h>
#include <stat.h>

#define _SYS0(T0, FN)                           \
    T0 FN(){                                    \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
            :"a"(NR_##FN)                       \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

#define _SYS1(T0, FN, T1)                       \
    T0 FN(T1 p1){                               \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
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
            :"=a"(r)                            \
            :"a"(NR_##FN),                      \
             "b"((int)p1),                      \
             "c"((int)p2)                       \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

#define _SYS3(T0, FN, T1, T2, T3)               \
    T0 FN(T1 p1, T2 p2, T3 p3){                 \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
            :"a"(NR_##FN),                      \
             "b"((int)p1),                      \
             "c"((int)p2),                      \
             "d"((int)p3)                       \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

extern int errno;

enum {
    //
    NR_setup, 
    //
    NR_fork,
    NR_exec,
    NR__exit,
    NR_pause,
    NR_wait,
    NR_waitpid,
    NR_chdir,
    NR_chroot,
    //
    NR_nice,
    NR_getpid,
    NR_getppid,
    NR_getuid,
    NR_getgid,
    NR_geteuid,
    NR_getegid,
    NR_getpgrp,
    NR_setpgrp,
    NR_setreuid,
    NR_setregid,
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
    NR_lseek,
    NR_link,
    NR_unlink,
    NR_mknod,
    NR_read,
    NR_write,
    NR_truncate,
    NR_fcntl,
    NR_mount,
    NR_umount,
    NR_stat,
    NR_fstat,
    NR_dup,
    NR_dup2,
    //
    NR_debug
};

int nosys       (struct trap *);
int sys_setup   (struct trap *);
//
int sys_access  (struct trap *);
int sys_open    (struct trap *);
int sys_close   (struct trap *);
int sys_lseek   (struct trap *);
int sys_creat   (struct trap *);
int sys_write   (struct trap *);
int sys_read    (struct trap *);
int sys_fcntl   (struct trap *);
int sys_mknod   (struct trap *);
int sys_dup     (struct trap *);
int sys_dup2    (struct trap *);
int sys_link    (struct trap *);
int sys_unlink  (struct trap *);
int sys_truncate(struct trap *);
int sys_stat    (struct trap *);
int sys_fstat   (struct trap *);
//
int sys_fork    (struct trap *);
int sys_exec    (struct trap *);
int sys__exit   (struct trap *);
int sys_debug   (struct trap *);
//
int sys_getpid  (struct trap *);
int sys_getppid (struct trap *);
int sys_getuid  (struct trap *);
int sys_getgid  (struct trap *);
int sys_geteuid (struct trap *);
int sys_getegid (struct trap *);
int sys_getpgrp (struct trap *);
int sys_setpgrp (struct trap *);
int sys_setreuid(struct trap *);
int sys_setregid(struct trap *);
//
int sys_chroot  (struct trap *);
int sys_chdir   (struct trap *);
int sys_nice    (struct trap *);
int sys_kill    (struct trap *);
int sys_signal  (struct trap *);
int sys_pause   (struct trap *);
int sys_wait    (struct trap *);
int sys_waitpid (struct trap *);
int sys_sigaction (struct trap *);
int sys_sigreturn (struct trap *);

static inline _SYS0(int, debug);
static inline _SYS2(int, access, char*, int);
static inline _SYS3(int, open, char*, int, int);
static inline _SYS2(int, creat, char*, int);
static inline _SYS1(int, close, int);
static inline _SYS3(int, fcntl, int, int, int);
static inline _SYS3(int, mknod, char*, int, int); 
static inline _SYS3(int, write, int, char*, int);
static inline _SYS3(int, read, int, char*, int);
static inline _SYS3(int, lseek, int, int, int);
static inline _SYS1(int, chdir, char*);
static inline _SYS1(int, chroot, char*);
static inline _SYS1(int, dup, int);
static inline _SYS2(int, dup2, int, int);
static inline _SYS2(int, link, char*, char*);
static inline _SYS1(int, unlink, char*); 
static inline _SYS2(int, stat, char*, struct stat*); 
static inline _SYS2(int, fstat, int, struct stat*); 
//
static inline _SYS0(int, setup);
static inline _SYS0(int, fork); 
static inline _SYS2(int, exec, char*, char**);
static inline _SYS1(int, _exit, int);
//
static inline _SYS1(int, nice, int);
static inline _SYS0(int, getpid);
static inline _SYS0(int, getppid);
static inline _SYS0(int, getuid);
static inline _SYS0(int, getgid);
static inline _SYS0(int, geteuid);
static inline _SYS0(int, getegid);
static inline _SYS0(int, getpgrp);
static inline _SYS0(int, setpgrp);
static inline _SYS2(int, setreuid, int, int);
static inline _SYS2(int, setregid, int, int);
//
static inline _SYS2(int, kill, int, int);
static inline _SYS2(int, signal, int, uint);
static inline _SYS3(int, sigaction, int, struct sigaction*, struct sigaction*);
static inline _SYS0(int, sigreturn);
static inline _SYS3(int, waitpid, int, int*, int);
static inline _SYS1(int, wait, int*);
static inline _SYS0(int, pause);

#endif
