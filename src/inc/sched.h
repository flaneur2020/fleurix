/**
 *
 * just misc right now
 *
 * _Fleurer<me.ssword@gmail.com>
 *
 * */ 

#ifndef SCHED_H
#define SCHED_H

struct proc {
    uchar               p_stat;
    uchar               p_flag;
    uint                p_pid;
    uint                p_ppid;
    uint               *p_textp;
    uint                p_tsize;
    struct seg_desc     p_ldt[3];
    struct tss_desc     p_tss;
};

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


#endif
