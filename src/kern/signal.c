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
#include <sig.h>

/* sig.c */

/* ----------------------------------------------------- */

/* returns a signal number, or returns 0 on no signal */
int signum(){
    uint n, sig;
    if (sig==0) 
        return 0;
    for (n=0; n<32; n++) {
        if (sig & (1<<n))
            return n;
    }    
}

/* returns true if the current process has got a signal. */
int issig(){
    uint n, sig;

    sig = signum();
    if (cu->p_signal[sig]!=SIG_IGN){
        return sig;
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

    n = signum();
    cu->p_sig &= ~(1<<n);
    if ((ufunc=cu->p_signal[n]) != SIG_DFL) {
        tf = cu->p_trap;
        esp = tf->esp;
        upush(&esp, &tf->eip, sizeof(uint));
        tf->esp = esp;
        tf->eip = ufunc;
        _retsys(cu->p_trap);
        return;
    }
    // on default
    // do_exit(1);
}

/* ----------------------------------------------------- */

/* send a signal to a process. */
int do_kill(int pid, int sig){
}

/* handle a signal to a user function */
int do_signal(int sig, sigfunc_t func){
}

int do_sigaction(int sig){
}
