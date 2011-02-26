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

/* returns true if the current process has got a signal. */
int issig(){
    uint n, sig;

    sig = cu->p_sig;
    if (sig==0) 
        return 0;
    for (n=0; n<32; n++) {
        if (sig & (1<<n)) {
            if (cu->p_signal[n]!=SIG_IGN) 
                return n;
        }
    }
    return 0;
}

/*
 * perform the action specified by the current signal.
 * the usual sequence is: 
 * if (n=issig()) 
 *      psig(n);
 * */
int psig(uint sig){
    cu->p_sig &= ~(1<<sig);
}

/* ----------------------------------------------------- */

/* send a signal to a process. */
int do_kill(int pid, int sig){
}

/* handle a signal to a user function */
int do_signal(int sig, sigfunc_t func){
}

