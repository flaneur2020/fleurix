/**
 *
 * just misc right now
 *
 * _Fleurer<me.ssword@gmail.com>
 *
 * */ 

#ifndef SCHED_H
#define SCHED_H

// stores all the data do not swaped out
struct proc {
    uchar       p_stat;
    uchar       p_flag;
    uint        p_pid;
    uint        p_ppid;
};

// all the data which might be swapped out
struct user {
    struct proc         *u_procp;
    struct seg_desc     u_ldt[3];
    struct tss_desc     u_tss;
};

#endif
