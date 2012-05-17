#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <signal.h>
#include <setjmp.h>

#include <unistd.h>

/*
 * sys4.c 
 * syscalls on signal
 *
 * */

/* kill(int pid, int sig); */
int sys_kill(struct trap *tf){
    int pid = (int)tf->ebx;
    int sig = (int)tf->ecx;
    
    return do_kill(pid, sig);
}

int sys_pause(struct trap *tf){
    sleep((uint)&sys_pause, PWAIT);
    return 0;
}

/* -------------------------------------------------- */

/* signal(int sig, void (*ufunc)(int)); */
int sys_signal(struct trap *tf){
    int sig = (int)tf->ebx;
    uint ufunc = (uint)tf->ecx;

    return do_signal(sig, (void*)ufunc);
}

/* sigaction(int sig, struct sigaction *sa, struct sigaction *old_sa); */
int sys_sigaction(struct trap *tf){
    int sig = (int) tf->ebx;
    struct sigaction *sa = (struct sigaction*) tf->ecx;
    struct sigaction *old_sa = (struct sigaction*)tf->edx;

    return do_sigaction(sig, sa, old_sa);
}

/* called on returning from the signal handler. 
 * note: this syscall should NEVER be called directly.
 * */
int sys_sigreturn(struct trap *tf){
    struct jmp_buf *ujbuf;
    ujbuf = (struct jmp_buf*)(tf->eip-46); // hard coded here, be careful
    tf->esp = ujbuf->esp;
    tf->eip = ujbuf->eip;
    tf->ebx = ujbuf->ebx;
    tf->edi = ujbuf->edi;
    tf->esi = ujbuf->esi;
    tf->ebp = ujbuf->ebp;
    cu->p_sigmask = ujbuf->__sigmask;
    return 0;
}

int sys_alarm(struct trap *tf){
    syserr(ENOSYS);
    return -1;
}

/* --------------------------- */

int sys_sync(struct trap *tf){
    syserr(ENOSYS);
    return -1;
}

