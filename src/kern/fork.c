#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <conf.h>                      

/*
 * proc.c 2010 fleurer
 * this file implies the initilize of proc[0] and routine fork(), 
 *
 * */

// one page size
uchar kstack0[0x1000] = {0, };        

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

/*
 * copy the current proc's address space into a new proc
 * return 0 on success
 * */
int copy_mem_to(struct proc *to){
    uint old_limit = get_seg_limit(&(cu->p_ldt[1])); 
    uint old_base  = get_seg_base(&(cu->p_ldt[1])); 
    uint new_base  = to->p_pid * PROCSIZ;
    set_seg(&(to->p_ldt[1]), new_base, old_limit, RING3, STA_X | STA_R);
    set_seg(&(to->p_ldt[2]), new_base, old_limit, RING3, STA_W);
    // copy page tables
    int ret = copy_vm(new_base, old_base, old_limit);
    asm("hlt");
    if (ret!=0){
        return -1;
    }
    return 0;
}

/*
 * main part of sys_fork()
 * */
int copy_proc(struct trap *tf){
    uint nr; 
    struct proc *p;
    struct trap *ntf;
    int fd;
    struct file *fp;
    
    nr = find_pid();
    if (nr==0){
        panic("no pid availible.");
    }

    p = (struct proc *) alloc_page(); 
    if (p==NULL){
        panic("no page availible.");
    }

    proc[nr] = p;
    *p = *cu;
    p->p_pid = nr;
    p->p_ppid = cu->p_pid;
    // on sche
    p->p_flag = cu->p_flag;
    p->p_cpu  = cu->p_cpu;
    p->p_pri  = cu->p_pri;
    p->p_nice = cu->p_nice;
    // increase the reference count of inodes, and dup files
    p->p_cdir = cu->p_cdir;
    p->p_cdir->i_count++;
    for (fd=0; fd<NOFILE; fd++){
        fp = cu->p_ofile[fd];
        if (fp != NULL) {
            fp->f_ino->i_count++;
            p->p_ofile[fd] = fp;
        }
    }
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

/* TODO: do_exit(). */
int do_exit(){
}

/* ----------------------------------------------------------- */

/* 
 * init proc[0] 
 * set the LDT and th ONLY TSS into GDT
 * and make current as proc[0]
 */
void sched_init(){
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
    printf("pid:%d pri:%d cpu:%d nice:%d stat:%d esp0:%x eip:%x \n", p->p_pid, p->p_pri, p->p_cpu, p->p_nice, p->p_stat, p->p_contxt.esp, p->p_trap->eip);
}


