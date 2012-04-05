#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
//
#include <page.h>
#include <vm.h>
//
#include <conf.h>


/*
 * The value of pid can be: (via `man waitpid`)
 *   < -1   wait for any child process whose process group ID is equal to the absolute value of pid.
 *   -1     wait for any child process.  
 *   0      wait for any child process whose  process group ID  is equal to that of the calling process.
 *   > 0    wait for the child whose process ID is equal to the value of pid.
 *
 * opt: 
 *   WNOHANG : return immediately if no child exited.  
 *
*/
int do_waitpid(int pid, int *stat, int opt){
    struct proc *p;
    uint nr; 

    if (vm_verify((uint)stat, sizeof(int)) < 0){
        syserr(EFAULT);
        return -1;
    }
    //
_repeat:
    for(nr=1; nr<NPROC; nr++){
        if ((p=proc[nr]) && p!=cu) {
            if (pid < -1) {
                if (p->p_ppid!=cu->p_pid || p->p_pgrp!=cu->p_pid) 
                    continue;
            }
            else if (pid == -1) {
                if (p->p_ppid!=cu->p_pid) 
                    continue;
            }
            else if (pid == 0) {
                if (p->p_ppid!=cu->p_pid || p->p_pgrp!=cu->p_pgrp) 
                    continue;
            }
            else if (pid > 0) {
                if (p->p_ppid!=cu->p_pid || p->p_pid!=pid) 
                    continue;
            }
            // on found
            switch(p->p_stat){
                case SZOMB:
                    *stat = p->p_ret;
                    pid = p->p_pid;
                    proc[pid] = NULL;
                    kfree(p, PAGE);
                    return pid;
                default:
                    continue;
            }
        }
    }

    // if no child has found
    if (opt & WNOHANG) {
        return 0;
    }
    sleep((uint)cu, PWAIT);
    goto _repeat;
}
