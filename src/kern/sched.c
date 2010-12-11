#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <conf.h>

// one page size, stores some info on proc[0] and its kernel stack
// note: inialized as 0.
uchar kstack0[1024] = {0, };

void _hwint_ret();

struct proc *proc[NPROC] = {NULL, };
struct proc *current = NULL;

struct tss_desc tss;

uint runrun = 0; 

/*******************************************************************************/

/*******************************************************************************/

/* mark a proccess SWAIT, commonly used on waiting a resource.
 * TODO: race condition should consider later.
 * */
void sleep(uint chan, int pri){
    cli();
    current->p_chan = chan;
    current->p_pri  = pri;
    current->p_stat = SWAIT;
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
    if (p->p_pri < current->p_pri){
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
            if (p->p_pri>PUSER) {
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

    // re-caculate current's p_pri
    setpri(current);

    // find the proc
    for(i=0;i<NPROC;i++){
        if ((p=proc[i]) && (p->p_stat==SRUN)) {
            if (p->p_pri < n){
                n = p->p_pri;
                np = p;
            }   
        }
    }
    if (np==NULL){
        np = proc[0];
    }
    //debug_proc_list();
    if (np!=current) {
        swtch_to(np);
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
int copy_mem_to(struct proc *to){
    uint old_limit = get_seg_limit(&(current->p_ldt[1])); 
    uint old_base  = get_seg_base(&(current->p_ldt[1])); 
    uint new_base  = to->p_pid * 0x4000000;
    set_seg(&(to->p_ldt[1]), new_base, old_limit, RING3, STA_X | STA_R);
    set_seg(&(to->p_ldt[2]), new_base, old_limit, RING3, STA_W);
    // copy page tables
    int ret = copy_vm(new_base, old_base, old_limit);
    if (ret!=0){
        return -1;
    }
    return 0;
}

/*
 * main part of sys_fork()
 * TODO: increment the reference count of inodes.
 * */
int copy_proc(struct trap *tf){
    uint nr; 
    struct proc *p;
    struct trap *ntf;
    
    nr = find_empty_pid();
    if (nr==0){
        panic("no pid availible.");
    }

    p = (struct proc *) alloc_page(); 
    if (p==NULL){
        panic("no page availible.");
    }

    proc[nr] = p;
    *p = *current;
    p->p_pid = nr;
    p->p_ppid = current->p_pid;
    // on sche
    p->p_flag = current->p_flag;
    p->p_cpu  = current->p_cpu;
    p->p_pri  = current->p_pri;
    p->p_nice = current->p_nice;
    // increase the reference count of inodes
    p->p_cdir = current->p_cdir;
    p->p_cdir->i_count++;
    // init the new proc's kernel stack
    p->p_trap = ntf = ((struct trap *)(uint)p + 0x1000) - 1;
    *ntf = *tf;
    ntf->eax = 0; // this is why fork() returns 0.
    // init the new proc's contxt for the first swtch.
    memset(&(p->p_contxt), 0, sizeof(struct contxt));
    p->p_contxt.eip = &_hwint_ret;
    p->p_contxt.esp = p->p_trap;
    // init ldt
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
    // on shedule
    p->p_cpu = 0;
    p->p_pri = 0;
    p->p_nice = 20;
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

/* move into user space(ring3) via an IRET and 
 * enable intrrupt.
 * */
void umode_init(){
    asm volatile( 
        "sti;" 
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
    printf("pid:%d pri:%d cpu:%d nice:%d stat:%d esp0:%x eip:%x \n", p->p_pid, p->p_pri, p->p_cpu, p->p_nice, p->p_stat, p->p_contxt.esp, p->p_trap->eip);
}


/***********************************************************************/


