#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>

// one page size, stores some info on proc[0] and its kernel stack
// note: inialized as 0.
uchar kstack0[1024] = {0, };

void _hwint_ret();

struct proc *proc[NPROC] = {NULL, };
struct proc *current = NULL;

struct tss_desc tss;

/*******************************************************************************/

/*******************************************************************************/

/* TODO: priority should be considered later. */
void sleep(uint chan){
    current->p_chan = chan;
    current->p_stat = SWAIT;
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

/*
 * invoked by do_timer() in timer.c;
 * Round Robbin right now.
 * */
void swtch(){
    struct proc *p;
    uint i = current->p_pid + 1;
_do_find:
    for (; i<NPROC; i++){
        if ((p = proc[i]) == NULL) continue;
        if (p->p_stat == SRUN){
            swtch_to(p);
            return;
        }
    }
    if (i==NPROC){
        i=0;
        goto _do_find;
    }
}

/*
 * do the switch task! In software.
 * */
void swtch_to(struct proc *to){
    struct proc *from;
    // change ldt & tss
    tss.esp0 = (uint)to+0x1000;
    lldt(_LDT(to->p_pid));
    from = current;
    current = to;
    _do_swtch(&(from->p_contxt), &(to->p_contxt));
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
    uint old_base  = get_seg_base(&(current->p_ldt[1])); 
    uint new_base  = p->p_pid * 0x4000000;
    p->p_textp     = new_base;
    p->p_tsize     = old_limit;
    set_seg(&(p->p_ldt[1]), new_base, old_limit, 3, STA_X | STA_R);
    set_seg(&(p->p_ldt[2]), new_base, old_limit, 3, STA_W);
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
int copy_proc(struct trap_frame *tf){
    uint nr; 
    struct proc *p;
    struct trap_frame *ntf;
    
    nr = find_empty_pid();
    if (nr==0){
        panic("copy_proc(): no pid availible.");
    }

    p = (struct proc *) palloc(); 
    if (p==NULL){
        panic("copy_proc(): no page availible.");
    }

    proc[nr] = p;
    p->p_pid = nr;
    p->p_ppid = current->p_pid;
    p->p_flag = current->p_flag;
    // init the new proc's kernel stack
    p->p_trap = ntf = ((struct trap_frame *)(uint)p + 0x1000) - 1;
    *ntf = *tf;
    ntf->eax = 0; // this is why fork() returns 0.
    // init the new proc's contxt for the first swtch.
    memset(&(p->p_contxt), 0, sizeof(struct contxt));
    p->p_contxt.eip = &_hwint_ret;
    p->p_contxt.esp = p->p_trap;

    set_ldt(&gdt[LDT0+p->p_pid], p->p_ldt);
    if (copy_mem_to(p) != 0){
        panic("copy_proc(): error on copy mem.");
    }

    p->p_stat = SRUN;
    return nr;
}


/*******************************************************************************/

/* 
 * init proc[0] 
 * set the LDT and th ONLY TSS into GDT
 * and make current as proc[0]
 */
void sched_init(){
    struct proc *p = current = proc[0] = (struct proc *)(uint) kstack0;
    p->p_pid = 0;
    p->p_ppid = 0;
    p->p_stat = SRUN;
    p->p_flag = SLOAD;
    // init tss
    tss.ss0  = KERN_DS;
    tss.esp0 = (uint)p + 0x1000;
    set_tss(&gdt[TSS0], &tss);
    ltr(_TSS);
    // init its ldt
    set_seg(&(p->p_ldt[1]), 0, 640*1024, 3, STA_X | STA_R);
    set_seg(&(p->p_ldt[2]), 0, 640*1024, 3, STA_W);
    // put proc0's LDT inside GDT & lldt
    set_ldt(&gdt[LDT0], &(p->p_ldt));
    lldt(_LDT(p->p_pid));
}

/*
 * Move into user space(ring3) by iret.
 * */
void umode_init(){
    asm volatile( 
        "mov $0x17, %ax;"
        "mov %ax, %ds;" 
        "mov %ax, %es;" 
        "mov %ax, %fs;"
        "mov %ax, %gs;" 
        "mov %esp, %eax;"
        "pushl $0x17;"
        "pushl %eax;"
        "pushf;"
        "pushl $0x0f;"
        "push $1f;"
        "iret;"
        "1:"
        );
}

/***********************************************************************/

void debug_proc_list(){
    int i;
    struct proc *p;
    for(i=0; i<NPROC; i++){
        p = proc[i];
        if(p){
            debug_proc(p);
        }
    }
}

void debug_proc(struct proc *p){
    printf("%s ", (p==current)? "-":" " );
    printf("pid:%d stat:%d esp0:%x\n", p->p_pid, p->p_stat, (uint)p+0x1000);
}


/***********************************************************************/


