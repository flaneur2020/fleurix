
#include <param.h>
#include <x86.h>
#include <sys.h>
#include <sched.h>

// one page size, store proc[0]'s user structure and its kernel stack
// NOTE: inialized as 0!
uchar           kpage[4096] = {0, };
struct user     *u = kpage;

struct proc     proc[NPROC] = {0, };

// init proc[0] 
// set the proc0's TSS & LDT inside GDT
void sched_init(){
    proc[0].p_pid = 0;
    proc[0].p_ppid = 0;
    proc[0].p_stat = SSTOP;
    proc[0].p_flag = 0;
    proc[0].p_userp = u;
    // init u
    u->u_procp = &proc[0];
    // init it's LDT
    set_seg(&(u->u_ldt[1]), 0, 4*1024*1024, 0, STA_X | STA_R);
    set_seg(&(u->u_ldt[2]), 0, 4*1024*1024, 0, STA_W);
    // init it's tss
    // NOTE: we assume this tss have been cleared as 0
    u->u_tss.esp0 = (uint)u + 4096;
    u->u_tss.ss0  = KERN_DS; 
    u->u_tss.cr3  = pdir;
    u->u_tss.es   = 0x17; //10111
    u->u_tss.cs   = 0x17;
    u->u_tss.ss   = 0x17;
    u->u_tss.ds   = 0x17;
    u->u_tss.fs   = 0x17;
    u->u_tss.gs   = 0x17;
    u->u_tss.ldt  = _LDT(0);
    u->u_tss.iomb = 0x80000000;
    // put proc0's TSS & LDT inside GDT
    set_tss(&gdt[TSS0], &(u->u_tss));
    set_ldt(&gdt[LDT0], &(u->u_ldt));
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
