/**
 *
 * just misc right now
 *
 * _Fleurer<me.ssword@gmail.com>
 *
 * */ 

#ifndef SYS_H
#define SYS_H


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
