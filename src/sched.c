#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>

// one page size, stores some info on proc[0] and its kernel stack
// NOTE: inialized as 0!
uchar           mem_p0[1024] = {0, };

struct proc     *proc[NPROC] = {NULL, };
struct proc     *current = NULL;

struct tss_desc tss;

/*******************************************************************************/

/*******************************************************************************/

/*
 * invoked by do_timer() in timer.c;
 * */
void do_sched(struct regs *r){
    printf("do sched \n");
    uint i;
    struct proc *p;
    for (i=0; i<NPROC; i++){
        p = proc[i];
        if (p){
            // TODO: if ok, just iret normally
            swtch(current, current);
        }
    }
}

/*
 * do the switch task! i still didn't catch the usage the TSS hardware stuff. software is better, people says.
 * switch the kernel task, and do a IRET. We assume that every task switching just occured in a trap routine,
 * and all the state stuff have been pushed in right place. Note that when a IRQ raised, CPU fetch the cs & 
 * eip from IDT, while ss0 & esp0 from the current TSS.
 * */
void swtch(struct proc *from, struct proc *to){
    printf("swtch: from %x to %x\n", from, to);
    // change ldt & tss
    tss.esp0 = (uint)to + 0x1000;
    lldt(_LDT(to->p_pid));
    current = to;
    asm volatile(
        "mov    %%eax, %%esp;"
        "jmp    _int_restore_regs;"
        ::"a"(to->p_regs));
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
    uint nr; 
    struct proc *p;
    
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
    p->p_ppid = current->p_ppid;
    p->p_flag = current->p_flag;
    // init kernel stack
    struct regs *trap = ((struct regs *)(uint)p + 0x1000) - 1;
    *trap = *r;
    trap->eax = 0;

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
    struct proc *p = current = proc[0] = (struct proc *)(uint) mem_p0;
    p->p_pid = 0;
    p->p_ppid = 0;
    p->p_stat = SSTOP;
    p->p_flag = 0;
    // init tss
    tss.ss0  = KERN_DS;
    tss.esp0 = (uint)p + 0x1000;
    set_tss(&gdt[TSS0], &tss);
    ltr(_TSS);
    // init its ldt
    set_seg(&(p->p_ldt[1]), 0, 0xffffffff, 3, STA_X | STA_R);
    set_seg(&(p->p_ldt[2]), 0, 0xffffffff, 3, STA_W);
    // put proc0's LDT inside GDT & lldt
    set_ldt(&gdt[LDT0], &(p->p_ldt));
    lldt(_LDT(p->p_pid));
}

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
    printf("pid:%d esp0:%x\n", p->p_pid, (uint)p+0x1000);
}


/***********************************************************************/
// load proc[n]'s TSS into tr
void ltr(uint n){
    asm volatile("ltr %%ax"::"a"(n));
}

void lldt(uint n){
    asm volatile("lldt %%ax"::"a"(n));
}

/*
 * ljmp seg, offset...Lovely little instruction.
 * But seg seems only availible for immediate value at compile time.
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

