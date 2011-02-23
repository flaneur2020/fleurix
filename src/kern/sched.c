#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <conf.h>

uint runrun = 0; 

/*******************************************************************************/

/* mark a proccess SWAIT, commonly used on waiting a resource.
 * TODO: race condition should consider later.
 * */
void sleep(uint chan, int pri){
    cli();
    cu->p_chan = chan;
    cu->p_pri  = pri;
    cu->p_stat = SWAIT;
    sti();
    swtch();
}

void wakeup(uint chan){
    struct proc *p;
    int i;
    for(i=0; i<NPROC; i++){
        if ((p = proc[i]) == NULL) continue;
        if (p->p_chan == chan) {
            setrun(p);
        }
    }
} 

void setrun(struct proc *p){
    p->p_chan = 0;
    p->p_stat = SRUN;
}

/*******************************************************************/

/* re-caculate the proc's p_pri
 * if it's lower than the current proc's p_pri, set the re-schedule flag.
 * */
void setpri(struct proc *p){
    int n;
    n = p->p_cpu/16 + PUSER + p->p_nice;
    if (n >= 127) n=127;
    if (n <= -126) n=-126;
    if ((p!=cu) && p->p_pri < cu->p_pri){
        runrun = 1;
    }
    p->p_pri = n;
}

/* called once per second, re-caculate all the procs' p_pri 
 * TODO: stil didn't catch the idea of traditional Unix's scheduling.
 * choose linus's approach instead later.
 * */
void sched_cpu(){
    struct proc *p;
    int i;
    for(i=0;i<NPROC;i++){
        if ((p=proc[i])) {
            p->p_cpu /= 2;
            if (p->p_pri > PUSER) {
                setpri(p);
            }
        }
    }
}

/* 
 * find the next proc and switch it.
 * re-caculate the current proc's p_pri on the end of time quatum.
 * */
void swtch(){
    int i;
    char n=127;
    struct proc *p=NULL, *np=NULL;

    // clear the re-schedule flag
    runrun = 0;
    // find the proc
    for(i=0;i<NPROC;i++){
        if ((p=proc[i]) && (p->p_stat==SRUN)) {
            if (p->p_pri <= n){
                n = p->p_pri;
                np = p;
            }
        }
    }
    if (np==NULL){
        n = proc[0]->p_pri;
        np = proc[0];
    }
    if (np!=cu) {
        swtch_to(np);
    }
}

/*
 * do the switch task! In software.
 * */
void swtch_to(struct proc *to){
    struct proc *from;
    tss.esp0 = (uint)to + PAGE; 
    from = cu;
    cu = to;
    lpgd(to->p_vm.vm_pgd);
    _do_swtch(&(from->p_contxt), &(to->p_contxt));
}
