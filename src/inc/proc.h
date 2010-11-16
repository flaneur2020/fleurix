#ifndef PROC_H
#define PROC_H

/* the regs saved in a task switch. */
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
    char                p_stat;
    char                p_flag;
    char                p_pri;          /* on shedule */
    char                p_cpu;          /* - */
    int                 p_nice;         /* - */
    int                 p_time;         /* on swap */
    uint                p_pid;          
    uint                p_ppid;         /* parent's pid */
    uint                p_textp;
    uint                p_tsize;
    uint                p_stack;
    uint                p_brk;
    uint                p_chan;         /* the event channel which proc is awaiting */
    uint                p_error;        /* return error code */
    struct seg_desc     p_ldt[3];
    struct contxt       p_contxt;
    struct trap        *p_trap;         /* the current trap on the task switch. */
};

extern struct proc *proc[NPROC];

extern struct proc *current;

/* stat codes */
#define SSLEEP      1           // sleeping on high priority
#define SWAIT       2           // sleeping on low  priority
#define SRUN        3           // running
#define SZOMB       4           // proc is being terminated
#define SSTOP       5           // proc is being traced

/* flag codes */
#define SLOAD       0x1         // in core
#define SSYS        0x2         // sheduling proc
#define SLOCK       0x4         // proc cannot be swapped
#define SSWAP       0x8         // proc is being swapped out
#define STRC        0x10        // proc is being traced
#define SWTED       0x20        // another tracing flag

/* priorities
 * probably should not be altered too much.
 */
#define	PSWP	  (-100)
#define	PINOD	  (-90)
#define	PRIBIO	(-50)
#define	PPIPE	  1
#define	PWAIT	  40
#define	PSLEP	  90
#define	PUSER	  100

// error code

#endif
