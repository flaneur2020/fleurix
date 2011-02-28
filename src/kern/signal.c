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
        if (sig & (1<<n)) {
            cu->p_sig &= ~(1<<n);
            if (cu->p_sigact[n].sa_handler == SIG_IGN)
                return 0;
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

    n = cu->p_cursig;
    if (n==0)
        return;
    cu->p_cursig = 0;
    if ((ufunc=cu->p_sigact[n].sa_handler) != SIG_DFL) {
        tf = cu->p_trap;
        esp = tf->esp;
        upush(&esp, &tf->eip, sizeof(uint));
        tf->esp = esp;
        tf->eip = ufunc;
        _retsys(cu->p_trap);
        return;
    }
    // on SIG_DFL
    do_exit(1);
}

/* ----------------------------------------------------- */

int sigsend(int pid, int sig){
    struct proc *p;
    
    p = proc[pid];
    if (p==NULL || sig<0 || sig>=NSIG) 
        return -1;
    if (p->p_sigact[sig].sa_handler != SIG_IGN) {
        p->p_sig |= (1<<sig);
        if (p->p_stat == SWAIT) {
            setrun(p);
        }
    }
    return 0;
}

/* ----------------------------------------------------- */

/* send a signal to a process. */
int do_kill(int pid, int sig){
}

/* handle a signal to a user function */
int do_signal(int sig, void (*func)(int)){
}

int do_sigaction(int sig, struct sigaction *sa, struct sigaction *old_sa){
}
