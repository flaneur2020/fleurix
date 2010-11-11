#ifndef PROC_H
#define PROC_H

struct contxt {
    int eip;
    int esp;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
    int ebp;
};

/*
 * struct proc indicated some info on sched.
 * */
struct proc {
    uchar               p_stat;
    uchar               p_flag;
    uint                p_chan;         /* the event the proc is awaiting */
    uint                p_error;        /* return error code */
    uint                p_pid;          
    uint                p_ppid;         /* parent's pid */
    uint                p_textp;
    uint                p_tsize;
    uint                p_stack;
    uint                p_brk;
    struct seg_desc     p_ldt[3];
    struct contxt       p_contxt;
    struct trap_frame  *p_trap;
};

extern struct proc *proc[NPROC];

extern struct proc *current;

// stat codes
#define SSLEEP      1           // sleeping on high priority
#define SWAIT       2           // sleeping on low  priority
#define SRUN        3           // running
#define SZOMB       4           // proc is being terminated
#define SSTOP       5           // proc is being traced

// flag codes
#define SLOAD       0x1         // in core
#define SSYS        0x2         // sheduling proc
#define SLOCK       0x4         // proc cannot be swapped
#define SSWAP       0x8         // proc is being swapped out
#define STRC        0x10        // proc is being traced
#define SWTED       0x20        // another tracing flag

// error code

#endif
