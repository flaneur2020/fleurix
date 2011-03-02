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
        if (sig & (1<<(n-1))) {
            // ignored signal
            if ((cu->p_sigact[n-1].sa_handler == SIG_IGN) && n!=SIGKILL) {
                cu->p_sig &= ~(1<<(n-1));
                return 0;
            }
            // normal case
            cu->p_sig &= ~(1<<(n-1));
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
    uint n, ufunc, esp;
    struct trap *tf;
    struct sigaction *sa;

    n = cu->p_cursig;
    if (n==0)
        return;
    sa = &(cu->p_sigact[n-1]);
    // check blocked signal
    if (cu->p_sigmask & (1<<(n-1)) && n!=SIGKILL)
        return 0;
    cu->p_sigmask |= sa->sa_mask;
    cu->p_cursig = 0;
    if ((ufunc=cu->p_sigact[n-1].sa_handler) != SIG_DFL) {
        tf = cu->p_trap;
        esp = tf->esp;
        upush(&esp, &tf->eip, sizeof(uint));
        upush(&esp, n, sizeof(uint));
        tf->esp = esp;
        tf->eip = ufunc;
        _retsys(cu->p_trap);
        return;
    }
    // on SIG_DFL
    do_exit(1);
}

/* ----------------------------------------------------- */

int sigsend(int pid, int n, int priv){
    struct proc *p;
    
    p = proc[pid];
    if (p==NULL || n<0 || n>=NSIG) {
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
        for (nr=0; nr<NPROC; nr++) {
            if (p=proc[nr] && (p!=cu) && (p->p_pgrp==cu->p_pid))
                ret = sigsend(nr, sig, 0);
        }
        return ret;
    }
    if (pid==-1) {
        for (nr=0; nr<NPROC; nr++) {
            if (p=proc[nr] && (p!=cu)) 
                ret = sigsend(nr, sig, 0);
        }
        return ret;
    }
    if (pid < -1) {
        for (nr=0; nr<NPROC; nr++) {
            if (p=proc[nr] && (p!=cu) && (p->p_pgrp==(-pid)))
                ret = sigsend(nr, sig, 0);
        }
        return ret;
    }
    syserr(EINVAL);
    return -1;
}

int do_sigaction(int sig, struct sigaction *sa, struct sigaction *old_sa){
    if (sig<1 || sig>NSIG || sig==SIGKILL)  {
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
int do_signal(int sig, void (*func)(int)){
}
