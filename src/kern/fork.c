#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <page.h>
#include <vm.h>

#include <conf.h>                      

/*
 * proc.c 2010 fleurer
 * this file implies the initilization of proc[0] and the implementation 
 * of fork(), 
 *
 * */

/* the kernel stack of proc0, one page size */
uchar kstack0[PAGE] __attribute__((aligned(PAGE)));        

struct proc *proc[NPROC] = {NULL, };
struct proc *cu = NULL;

struct tss_desc tss;              

extern void _hwint_ret();

/* ----------------------------------------------------------------- */

/*
 * find an empty proc slot, return the number as pid
 * return 0 on fail
 * */
int find_pid(){
    int nr;
    for(nr=0; nr<NPROC; nr++){
        if (proc[nr]==NULL){
            return nr;
        }
    }
    return 0;
}

/* Spawn a kernel thread.    
 * This is not quite cool, but we have to do some initialization in
 * kernel's address space, the approach in linux0.11 is not quite 
 * ease here for the fact that trap occured in the kernel space do 
 * not refering the esp in TSS.
 *
 * returns a pointer to the newly borned proc, one page size(with the kernel stack).
 * */
struct proc* kspawn(void (*func)()){
    uint nr;
    int fd;
    struct file *fp;
    struct proc *p;

    nr = find_pid();
    if (nr == 0) {
        panic("no free pid");
    }

    p = (struct proc *) kmalloc(PAGE);
    if (p==NULL) {
        panic("no free page");
    }
    proc[nr] = p;
    p->p_stat = SSLEEP; // set SRUN later.
    p->p_pid  = nr;
    p->p_ppid = cu->p_pid;
    p->p_flag = cu->p_flag;
    p->p_cpu  = cu->p_cpu;
    p->p_nice = cu->p_nice;
    p->p_pri  = PUSER;
    //
    p->p_uid  = cu->p_uid;
    p->p_gid  = cu->p_gid;
    p->p_ruid = cu->p_ruid;
    p->p_rgid = cu->p_rgid;
    // increase the reference count of inodes, and dup files
    p->p_wdir = cu->p_wdir;
    p->p_wdir->i_count++;
    p->p_iroot = cu->p_iroot;
    p->p_iroot->i_count++;
    // dup the files.
    for (fd=0; fd<NOFILE; fd++){
        fp = cu->p_ofile[fd];
        if (fp != NULL) {
            fp->f_ino->i_count++;
            p->p_ofile[fd] = fp;
        }
    }
    // clone kernel's address space.
    vm_clone(&p->p_vm);
    p->p_contxt = cu->p_contxt;
    p->p_contxt.eip = func;
    p->p_contxt.esp = (uint)p+PAGE;
    p->p_stat = SRUN;
    return p;
}

/*
 * main part of sys_fork().
 * note that the fact that ALL process swtching occurs in kernel
 * space, hence fork() just returns to _hwint_ret(in entry.S.rb),
 * and initialize a kernel stack just as intrrupt occurs here.
 * */
int do_fork(struct trap *tf){
    struct proc *p;
    struct trap *ntf;

    p = kspawn(&_hwint_ret);
    ntf = (struct trap *)((uint)p+PAGE) - 1;
    *ntf = *tf;
    ntf->eax = 0; // this is why fork() returns 0.
    p->p_contxt.esp = ntf;
    return p->p_pid;
}

/* ----------------------------------------------------------- */

/* 
 * init proc[0] 
 * set the LDT and th ONLY TSS into GDT
 * and make current as proc[0]
 */
void proc0_init(){
    struct proc *p = cu = proc[0] = (struct proc *)(uint) kstack0;
    p->p_pid = 0;
    p->p_ppid = 0;
    p->p_stat = SRUN;
    p->p_flag = SLOAD;
    // on shedule
    p->p_cpu = 0;
    p->p_pri = 0;
    p->p_nice = 20;
    // on user
    p->p_uid = 0;
    p->p_gid = 0;
    // attach the page table
    p->p_vm.vm_pgd = pgd0;
    // init tss
    tss.ss0  = KERN_DS;
    tss.esp0 = (uint)p + PAGE;
    // init its ldt
}

/* --------------------------------------------------- */

void dump_procs(){
    int i;
    struct proc *p;
    for(i=0; i<NPROC; i++){
        p = proc[i];
        if(p){
            dump_proc(p);
        }
    }
}

void dump_proc(struct proc *p){
    printf("%s ", (p==cu)? "-":" " );
    printf("pid:%d pri:%d cpu:%d nice:%d stat:%d esp0:%x eip:%x \n", p->p_pid, p->p_pri, p->p_cpu, p->p_nice, p->p_stat, p->p_contxt.esp, p->p_contxt.eip);
}


