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

#define KERN_CS 0x08
#define KERN_DS 0x10

// In main.c
void    *memcpy     (void *dest, void *src, uint count);
void    *memset     (void *dest, char val, uint count);
short   *memsetw    (short *dest, short val, uint count);
uint    strlen(char *str);

// In video.c
void    video_init  ();
void    cls         ();
void    putch       (char ch);
void    puts        (char *ch);
void    printf      (char *fmt, ...);

// In idt.c

void    idt_init();

// timer.c
void    do_timer();

#endif
