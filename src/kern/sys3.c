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
    if (n > 20) n = 20;
    if (n<-19) n=-19;
}

int sys_getpid(struct trap *tf){
    return cu->p_pid;
}

int sys_getuid(struct trap *tf){
    return cu->p_uid;
}

int sys_getgid(struct trap *tf){
    return cu->p_gid;
}

int sys_setgid(struct trap *tf){
    int gid = tf->ebx;

    if ((gid==cu->p_gid) || suser()){
        cu->p_gid = gid;
        cu->p_rgid = gid;
    }
}

int sys_setuid(struct trap *tf){
    int uid = tf->ebx;

    if ((uid==cu->p_uid) || suser()) {
        cu->p_uid = uid;
        cu->p_ruid = uid;
    }
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

