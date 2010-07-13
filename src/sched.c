
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>

// one page size, stores some info on proc[0] and its kernel stack
// NOTE: inialized as 0!
uchar           mem_p0[4096] = {0, };

struct proc     *proc[NPROC] = {NULL, };
struct proc     *current;


int find_empty_pid();

/*******************************************************************************/

/*******************************************************************************/

/*
 * invoked by do_timer() in timer.c;
 * */
void do_sched(struct regs *r){
}

void switch_to(struct proc *p, struct regs *r){
}

/*******************************************************************************/

/*
 * find an empty proc slot, return the number as pid
 * return 0 on fail
 * */
int find_empty_pid(){
    int nr;
    for(nr=0; nr<NPROC; nr++){
        if (proc[nr]==NULL){
            return nr;
        }
    }
    return 0;
}

/*
 * copy the current proc's address space into a new proc
 * return 0 on success
 * */
int copy_mem_to(struct proc *p){
    uint old_limit = get_seg_limit(&(current->p_ldt[1])); 
    uint old_base  = get_seg_base (&(current->p_ldt[1]));
    uint new_base  = p->p_pid * 0x4000000;
    p->p_textp     = new_base;
    p->p_tsize     = old_limit;
    set_seg(&(p->p_ldt[1]), new_base, old_limit, 0, STA_X | STA_R);
    set_seg(&(p->p_ldt[2]), new_base, old_limit, 0, STA_W);

    // copy page tables
    int ret = copy_ptab(old_base, new_base, old_limit);
    if (ret!=0){
        return -1;
    }
    return 0;
}

/*
 * main part of sys_fork()
 * */
int copy_proc(struct regs *r){
}

/*******************************************************************************/

/* 
 * init proc[0] 
 * set the proc0's TSS & LDT inside GDT
 * and make current as proc[0]
 */
void sched_init(){
}

/*
 * move to user mode by iret
 * */
void umode_init(){
}

/***********************************************************************/
// load proc[n]'s TSS into tr
void ltr(uint n){
    asm("ltr %%ax"::"a"(n));
}

void lldt(uint n){
    asm("lldt %%ax"::"a"(n));
}

/*
 * ljmp seg, offset...Lovely little instruction.
 * But seg seems only availible for immediate value in compile time.
 * Tricks needed, *Sucks*.
 * */
void ljmp(ushort seg, uint offset){
    struct{ uint offset, seg } _tmp;
    _tmp.offset = offset;
    _tmp.seg    = seg;
    asm volatile(
        "ljmp %0"
        ::"m"(_tmp)
        );
}

void lcall(ushort seg, uint offset){
    struct{ uint offset, seg } _tmp;
    _tmp.offset = offset;
    _tmp.seg    = seg;
    asm volatile(
        "lcall %0"
        ::"m"(_tmp)
        );
}

