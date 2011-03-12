#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>
//
#include <buf.h>
#include <inode.h>
#include <super.h>
#include <file.h>
//
#include <signal.h>
#include <setjmp.h>

void _usigret();

/* sig.c */

/* ----------------------------------------------------- */

/* returns true if the current process has got a signal.
 * on delivering the signal, clear the bit in cu->p_sig.
 * */
int issig(){
    uint n, sig;
    struct sigaction sa;

    sig = cu->p_sig;
    if (sig==0)
        return 0;
    for (n=0; n<32; n++) {
        if ((sig & (1<<(n-1))) && cu->p_pid!=0) {
            // ignored signal
            if ((cu->p_sigact[n-1].sa_handler == SIG_IGN) && n!=SIGKILL) {
                cu->p_sig &= ~(1<<(n-1));
                return 0;
            }
            // blocked
            if (cu->p_sigmask & (1<<(n-1)) && n!=SIGKILL)
                return 0;
            // normal case
            cu->p_cursig = n;
            return n;
        }
    }
    return 0;
}

/*
 * perform the action specified by the current signal.
 * the usual sequence is: 
 * if (issig()) 
 *      psig();
 * */
void psig(){
    uint n, ufunc, esp, usr;
    struct trap *tf;
    struct sigaction *sa;
    struct jmp_buf jbuf, *ujbuf;

    n = cu->p_cursig;
    if (n==0 || n>NSIG)
        return;
    cu->p_sig &= ~(1<<(n-1));
    sa = &(cu->p_sigact[n-1]);
    // check blocked signal
    if ((sa->sa_flags & SA_NOMASK)==0) {
        cu->p_sigmask |= sa->sa_mask;
    }
    cu->p_cursig = 0;
    if (sa->sa_handler != SIG_DFL) {
        tf = cu->p_trap;
        // save registers and the old mask 
        usigsav(&jbuf, tf, cu->p_sigmask);
        // push to the user stack, with a "shellcode"
        esp = tf->esp;
        usr = upush(&esp, &_usigret, 16);
        ujbuf = upush(&esp, &jbuf, sizeof(struct jmp_buf));
        upush(&esp, &n, sizeof(uint));
        upush(&esp, &usr, sizeof(uint));
        ufunc = sa->sa_handler;
        if (sa->sa_flags & SA_ONESHOT) {
            sa->sa_handler = NULL;
        }
        _retu(ufunc, esp);
        return;
    }
    // on SIG_DFL
    switch(n){
        // ignored on default
        case SIGCHLD:
        case SIGCONT:
            return;
        // exited on default
        case SIGSEGV:
            printk("seg fault.\n");
        case SIGINT:
        case SIGKILL:
        default:
            do_exit(1);
    }
}


/* ---------------------------------------------------- */

/* this code will be pushed onto user's stack, only to copy, never
 * call this routine manually. */
void _usigret() {
    asm("int $0x80;"::"a"(NR_sigreturn));
}

/* save the user state, which restored on sys_sigreturn(). */
void usigsav(struct jmp_buf *buf, struct trap *tf, uint mask){
    // save esp and eip
    buf->esp = tf->esp;
    buf->eip = tf->eip;
    // save callee-saved registers
    buf->ebx = tf->ebx;
    buf->ecx = tf->ecx;
    buf->edx = tf->edx;
    buf->edi = tf->edi;
    buf->esi = tf->esi;
    buf->ebp = tf->ebp;
    // sigmask
    buf->__sigmask = mask;
}

/* ----------------------------------------------------- */
int sigsend(int pid, int n, int priv){
    struct proc *p;
    
    p = proc[pid];
    if (pid==0 || p==NULL || n<0 || n>=NSIG) {
        syserr(EINVAL);
        return -1;
    }
    if ((priv || cu->p_uid==p->p_uid || suser())==0) {
        syserr(EPERM);
        return -1;
    }
    if (p->p_sigact[n-1].sa_handler==SIG_IGN && n!=SIGKILL) 
        return -1;
    p->p_sig |= (1<<(n-1));
    if (p->p_stat == SWAIT) {
        setrun(p);
    }
    return 0;
}

/* ----------------------------------------------------- */

/*
 * If pid is positive, then signal sig is sent to the process with the ID specified by pid.
 * If pid equals 0, then sig is sent to every process in the process group of the calling process.
 * If pid equals -1, then sig is sent to every process for which the calling process has permission to send signals, except for process 1 (init), but see below.
 * If pid is less than -1, then sig is sent to every process in the process group whose ID is -pid.
 * If sig is 0, then no signal is sent, but error checking is still performed; this can be used to check for the existence of a process ID or process group ID.
 * */
int do_kill(int pid, int sig){
    struct proc *p;
    int nr, ret;

    if (pid>0) {
        return sigsend(pid, sig, 0);
    }
    if (pid==0) {
        for (nr=1; nr<NPROC; nr++) {
            if (p=proc[nr] && (p!=cu) && (p->p_pgrp==cu->p_pid))
                ret = sigsend(nr, sig, 0);
        }
        return ret;
    }
    if (pid==-1) {
        for (nr=1; nr<NPROC; nr++) {
            if (p=proc[nr] && (p!=cu)) 
                ret = sigsend(nr, sig, 0);
        }
        return ret;
    }
    if (pid < -1) {
        for (nr=1; nr<NPROC; nr++) {
            if (p=proc[nr] && (p!=cu) && (p->p_pgrp==(-pid)))
                ret = sigsend(nr, sig, 0);
        }
        return ret;
    }
    syserr(EINVAL);
    return -1;
}

int do_sigaction(int sig, struct sigaction *sa, struct sigaction *old_sa){
    if (sig<1 || sig>NSIG || sig==SIGKILL || sa==NULL)  {
        syserr(EINVAL);
        return -1;
    }
    // store the old struct sigaction 
    if (old_sa!=NULL) {
        if (vm_verify(old_sa, sizeof(struct sigaction))<0) {
            syserr(EFAULT);
            return -1;
        }
        *old_sa = cu->p_sigact[sig-1];
    }
    // set the new sigaction
    if (vm_verify(sa, sizeof(struct sigaction))<0) {
        syserr(EFAULT);
        return -1;
    }
    cu->p_sigact[sig-1] = *sa;
    cu->p_sigact[sig-1].sa_mask |= (1<<(sig-1));
    return 0;
}

/* handle a signal to a user function */
int do_signal(int sig, void (*ufunc)(int)){
    struct sigaction *sa;

    if (sig<1 || sig>NSIG || sig==SIGKILL) {
        syserr(EINVAL);
        return -1;
    }
    sa = &cu->p_sigact[sig-1];
    sa->sa_handler = ufunc;
    sa->sa_flags = SA_ONESHOT | SA_NOMASK;
    sa->sa_mask = 0;
    cu->p_sigmask = 0;
    return 0;
}
