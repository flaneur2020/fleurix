
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>

// one page size, stores some info on proc[0] and its kernel stack
// NOTE: inialized as 0!
uchar           mem_p0[4096] = {0, };

struct proc     *proc[NPROC] = {NULL, };
struct proc     *current;


/*******************************************************************************/

/*
 * invoked by do_timer() in timer.c;
 * */
void do_sched(struct regs *r){
    int nr;
    struct proc *p;
    for(nr=0; nr<NPROC; nr++){
        p = proc[nr];
        if (p==NULL) continue;
        if (p==current){ continue; }
        switch_to(p, r);
    }
}

void switch_to(struct proc *p, struct regs *r){
    if (p==current){
        return;
    }

    return;
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
    //printf("base %x\n", new_base);
    //printf("size %d\n", old_limit);
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
    int nr, ret;
    struct proc *p;
    
    nr = find_empty_pid();
    if (nr==0){
        return -1;
    }

    p = (struct proc *)palloc(); 
    if (p==0){
        return -1;
    }

    proc[nr] = p;
    p->p_pid   = nr;
    p->p_ppid  = current->p_pid;
    
    // init tss & ldt stuff
    p->p_tss.link   = 0;
    p->p_tss.esp0   = (uint)p + 0x1000;
    p->p_tss.ss0    = 0x10;
    p->p_tss.eip    = r->eip;
    p->p_tss.eflags = r->eflags;
    p->p_tss.eax    = 0; //NOTE: this is why fork() returns 0
    p->p_tss.ebx    = r->ebx;
    p->p_tss.ecx    = r->ecx;
    p->p_tss.edx    = r->edx;
    p->p_tss.esp    = r->esp;
    p->p_tss.ebp    = r->ebp;
    p->p_tss.esi    = r->esi;
    p->p_tss.edi    = r->edi;
    p->p_tss.es     = r->es & 0xffff;
    p->p_tss.cs     = r->cs & 0xffff;
    p->p_tss.ss     = r->ss & 0xffff;
    p->p_tss.ds     = r->ds & 0xffff;
    p->p_tss.fs     = r->fs & 0xffff;
    p->p_tss.gs     = r->gs & 0xffff;
    p->p_tss.ldt    = _LDT(nr);
    p->p_tss.iomb   = 0x80000000;

    // set gdt
    set_tss(&gdt[TSS0+nr*2], &(p->p_tss));
    set_ldt(&gdt[LDT0+nr*2], &(p->p_ldt));
    
    // copy the address space
    ret = copy_mem_to(p);
    if (ret!=0){
        pfree(p);
        proc[0] = NULL;
        return -1;
    }

    // set SRUN at last. just in case
    p->p_stat = SRUN;

    return nr;
}

/*******************************************************************************/

// init proc[0] 
// set the proc0's TSS & LDT inside GDT
// and make current as proc[0]
void sched_init(){
    struct proc *p = current = proc[0] = mem_p0;
    p->p_pid = 0;
    p->p_ppid = 0;
    p->p_stat = SSTOP;
    p->p_flag = 0;
    // init it's tss
    // NOTE: we assume this tss have been cleared as 0
    p->p_tss.esp0 = (uint)p + 4096;
    p->p_tss.ss0  = KERN_DS; 
    p->p_tss.cr3  = pdir;
    p->p_tss.es   = 0x14; //10100
    p->p_tss.cs   = 0x12;
    p->p_tss.ss   = 0x14;
    p->p_tss.ds   = 0x14;
    p->p_tss.fs   = 0x14;
    p->p_tss.gs   = 0x14;
    p->p_tss.ldt  = _LDT(0);
    p->p_tss.iomb = 0x80000000;
    // put proc0's TSS & LDT inside GDT
    set_tss(&gdt[TSS0], &(p->p_tss));
    set_ldt(&gdt[LDT0], &(p->p_ldt));
    // init it's LDT
    set_seg(&(p->p_ldt[1]), 0, 420*1024, 0, STA_X | STA_R);
    set_seg(&(p->p_ldt[2]), 0, 420*1024, 0, STA_W);
    // load to TR and LDTR
    ltr(_TSS(0));
    lldt(_LDT(0));
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
        ::"m"(*&_tmp.offset), "m"(*&_tmp.seg)
        );
}
