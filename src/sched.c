
#include <param.h>
#include <x86.h>
#include <sys.h>
#include <sched.h>

// one page size, stores some info on proc[0] and its kernel stack
// NOTE: inialized as 0!
uchar           mem_p0[4096] = {0, };

struct proc     *proc[NPROC] = {NULL, };
struct proc     *current;

/*******************************************************************************/

void copy_proc(int nr, struct regs *r){
    struct proc *p;

    // TODO: have a check here
    p = (struct proc *)palloc(); 
    proc[nr] = p;
    p->p_pid   = nr;
    p->p_ppid  = current->p_pid;

    p->p_tss.link   = 0;
    p->p_tss.esp0   = (uint)p + 0x1000;
    p->p_tss.ss0    = 0x10;
    p->p_tss.eip    = r->eip;
    p->p_tss.eflags = r->eflags;
    p->p_tss.eax    = 0;
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
    // init it's LDT
    set_seg(&(p->p_ldt[1]), 0, 4*1024*1024, 0, STA_X | STA_R);
    set_seg(&(p->p_ldt[2]), 0, 4*1024*1024, 0, STA_W);
    // init it's tss
    // NOTE: we assume this tss have been cleared as 0
    p->p_tss.esp0 = (uint)p + 4096;
    p->p_tss.ss0  = KERN_DS; 
    p->p_tss.cr3  = pdir;
    p->p_tss.es   = 0x17; //10111
    p->p_tss.cs   = 0x17;
    p->p_tss.ss   = 0x17;
    p->p_tss.ds   = 0x17;
    p->p_tss.fs   = 0x17;
    p->p_tss.gs   = 0x17;
    p->p_tss.ldt  = _LDT(0);
    p->p_tss.iomb = 0x80000000;
    // put proc0's TSS & LDT inside GDT
    set_tss(&gdt[TSS0], &(p->p_tss));
    set_ldt(&gdt[LDT0], &(p->p_ldt));
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
