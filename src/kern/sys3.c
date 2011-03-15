#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <unistd.h>

/*
 * sys3.c
 * misc syscalls.
 * 
 * */

int sys_nice(struct trap *tf){
    int n = tf->ebx & 0xff;
    if (n > 20) n = 40;
    if (n < -39) n = -39;
}

int sys_getpid(struct trap *tf){
    return cu->p_pid;
}

int sys_getppid(struct trap *tf){
    return cu->p_ppid;
}

int sys_getpgrp(struct trap *tf){
    return cu->p_pgrp;
}

int sys_getuid(struct trap *tf){
    return cu->p_ruid;
}

int sys_getgid(struct trap *tf){
    return cu->p_rgid;
}

int sys_geteuid(struct trap *tf){
    return cu->p_euid;
}

int sys_getegid(struct trap *tf){
    return cu->p_egid;
}

/* int setreuid(int ruid, int euid); */
int sys_setreuid(struct trap *tf){
    int ruid = tf->ebx;
    int euid = tf->ecx;
    int old_ruid;

    old_ruid = cu->p_ruid;
    if (ruid > 0) {
        if ((cu->p_euid==ruid) || (old_ruid==ruid) || suser())
            cu->p_ruid = ruid;
        else 
            return syserr(EPERM);
    }
    if (euid > 0) {
        if ((cu->p_euid==euid) || (old_ruid==euid) || suser())
            cu->p_euid = euid;
        else 
            return syserr(EPERM);
    }
    return 0;
}

/* int setregid(int rgid, int egid); */
int sys_setregid(struct trap *tf){
    int rgid = tf->ebx;
    int egid = tf->ecx;

    if (rgid > 0) {
        if ((cu->p_rgid==rgid) || suser()) 
            cu->p_rgid = rgid;
        else 
            return syserr(EPERM);
    }
    if (egid > 0) {
        if (cu->p_egid==egid || cu->p_rgid==egid || suser()) 
            cu->p_egid = egid;
        else 
            return syserr(EPERM);
    }
    return 0;
}

/* int setpgrp(void); */
int sys_setpgrp(struct trap *tf){
    cu->p_pgrp = cu->p_pid;
}

/* -------------------------------- */

int sys_umask(struct trap *tf){
    int mask = tf->ebx & 0777;
    int old;

    old = cu->p_umask;
    cu->p_umask = mask;
    return old;
}

/* -------------------------------- */

int sys_stime(struct trap *tf){
}

int sys_utime(struct trap *tf){
}

int sys_time(){
}

