#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <unistd.h>

/*
 * sys4.c 
 * syscalls on signal
 *
 * */

int pause(struct trap *tf){
}

/* kill(int pid, int sig); */
int kill(struct trap *tf){
}

int signal(struct trap *tf){
}

int wait(struct trap *tf){
}

int alarm(struct trap *tf){
}

/* --------------------------- */

int sync(struct trap *tf){
    syserr(ENOSYS);
}

