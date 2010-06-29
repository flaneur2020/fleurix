/**
 *
 * just misc right now
 *
 * _Fleurer<me.ssword@gmail.com>
 *
 * */ 

#ifndef SYS_H
#define SYS_H

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;

#define NULL ((void*)0)

#define NPROC 128


#define KERN_CS 0x08
#define KERN_DS 0x10
#define USER_CS 0x18
#define USER_DS 0x20

// Kernel is 1mb, which locates in 0~LO_MEM
// main Mem(which procs live in) is 63mb, locates in LO_MEM~HI_MEM 
#define LO_MEM 0x100000
#define HI_MEM 0x4000000

#define NFRAME ((HI_MEM-LO_MEM)/4096)

// In main.c
void    *memcpy     (void *dest, void *src, uint count);
void    *memset     (void *dest, char val, uint count);
short   *memsetw    (short *dest, short val, uint count);
uint    strlen      (char *str);

// In video.c
void    video_init  ();
void    cls         ();
void    putch       (char ch);
void    puts        (char *ch);
void    printf      (char *fmt, ...);

// In idt.c
void    idt_init();

// In page.c
uint    palloc();
uint    pfree       (uint addr);

// timer.c
void    do_timer();

#endif
