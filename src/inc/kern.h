/**
 *
 * just misc right now
 *
 * _Fleurer<me.ssword@gmail.com>
 *
 * */ 

#ifndef KERN_H
#define KERN_H


// main.c
void    *memcpy(void *dest, void *src, uint count);
void    *memset(void *dest, char val, uint count);
short   *memsetw(short *dest, short val, uint count);
uint    strlen(char *str);

// video.c
void    video_init();
void    cls();
void    putch(char ch);
void    puts(char *ch);
void    printf(char *fmt, ...);

// idt.c
void    idt_init();

// seg.c
uint    get_seg_limit(struct seg_desc *seg);

extern  struct seg_desc     gdt[];

// page.c
uint    palloc();
int     pfree(uint addr);

extern  uint                *pdir;

// timer.c
void    do_timer();

// syscall.c
void    do_syscall();

// sched.c 
extern  struct proc         *current;

void    do_sched();

void    ltr(uint n);
void    lldt(uint n);
void    ljmp(ushort seg, uint offset);

#endif
