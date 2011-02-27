#ifndef PROC_H
#define PROC_H

#include <inode.h>
#include <file.h>
#include <vm.h>
#include <signal.h>

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
    uint                p_chan;         /* the event channel which proc is awaiting */
    int                 p_pri;          /* on shedule */
    int                 p_cpu;          /* - */
    int                 p_nice;         /* - */
    int                 p_time;         /* on swap */
    uint                p_pid;          
    uint                p_ppid;         /* parent's pid */
    uint                p_uid;          /* effective uid */
    uint                p_gid;          /* effective gid */
    uint                p_ruid;         /* real uid */
    uint                p_rgid;         /* real gid */
    uint                p_umask;        /* umask for files */
    uint                p_error;        /* return error code */
    uint                p_sig;          /* a bitmap of the recieved signals of current proc */
    struct sigaction    p_sigact[NSIG]; /* signal handlers */
    struct inode       *p_wdir;         /* current working dir */
    struct inode       *p_iroot;        /* the root dir */
    struct file        *p_ofile[NOFILE];/* file desciptors of the current opened files */
    struct vm           p_vm;
    struct contxt       p_contxt;       /* - */
    struct trap        *p_trap;         /* saved on entering kernel from user, for psig(). */
};

extern struct proc *proc[NPROC];

extern struct proc *cu;

/* flag for re-scheduling */
extern uint runrun; 

/* stat codes */
#define SSLEEP      1           // sleeping on high priority (unitterruptible)
#define SWAIT       2           // sleeping on low  priority (interruptible)
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
#define	PRIBIO	  (-50)
#define PRITTY    (-45)
#define	PPIPE	  1
#define	PWAIT	  40
#define	PSLEP	  90
#define	PUSER	  100


/* error codes */
#define ERROR		99
#define EPERM		 1
#define ENOENT		 2
#define ESRCH		 3
#define EINTR		 4
#define EIO		    5
#define ENXIO		 6
#define E2BIG		 7
#define ENOEXEC		 8
#define EBADF		 9
#define ECHILD		10
#define EAGAIN		11
#define ENOMEM		12
#define EACCES		13
#define EFAULT		14
#define ENOTBLK		15
#define EBUSY		16
#define EEXIST		17
#define EXDEV		18
#define ENODEV		19
#define ENOTDIR		20
#define EISDIR		21
#define EINVAL		22
#define ENFILE		23
#define EMFILE		24
#define ENOTTY		25
#define ETXTBSY		26
#define EFBIG		27
#define ENOSPC		28
#define ESPIPE		29
#define EROFS		30
#define EMLINK		31
#define EPIPE		32
#define EDOM		33
#define ERANGE		34
#define EDEADLK		35
#define ENAMETOOLONG	36
#define ENOLCK		37
#define ENOSYS		38
#define ENOTEMPTY	39

#endif
