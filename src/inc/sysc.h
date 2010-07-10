/**
 *
 * just misc right now
 *
 * _Fleurer<me.ssword@gmail.com>
 *
 * */

#ifndef SYSC_H
#define SYSC_H


#define _syscall0(NR, T0, FN)                   \
    T0 FN(){                                    \
        register r;                             \
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
        register r;                             \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
            :"a"(__NR),                         \
             "b"((uint)p1)                      \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }

#define _syscall2(NR, T0, FN, T1, T2)           \
    T0 FN(T1 p1, T2 p2){                        \
        register r;                             \
        asm volatile(                           \
            "int $0x80"                         \
            :"=a"(r)                            \
            :"a"(NR),                           \
             "b"((uint)p1),                     \
             "c"((uint)p2)                      \
        );                                      \
        if (r<0){                               \
            errno = -r;                         \
            return -1;                          \
        }                                       \
        return r;                               \
    }


#endif
