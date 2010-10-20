/**
 *
 * macro helpers on doing syscall.
 *
 * fleurer<me.ssword@gmail.com>
 *
 * */

#ifndef SYSC_H
#define SYSC_H


#define _syscall0(NR, T0, FN)                   \
    T0 FN(){                                    \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
            :"a"(NR)                            \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

#define _syscall1(NR, T0, FN, T1)               \
    T0 FN(T1 p1){                               \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
            :"a"(NR),                           \
             "b"((int)p1)                       \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

#define _syscall2(NR, T0, FN, T1, T2)           \
    T0 FN(T1 p1, T2 p2){                        \
        register int r;                         \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
            :"a"(NR),                           \
             "b"((int)p1),                      \
             "c"((int)p2)                       \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

extern int errno;

static inline _syscall0(0, int, debug); 
static inline _syscall1(1, int, putn, int);
static inline _syscall0(2, int, fork); 

#endif
