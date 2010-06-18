/**
 *
 * just misc right now
 *
 * */ 

#ifndef SYS_H
#define SYS_H

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;

#define KERN_CS 0x08
#define KERN_DS 0x10

#define IRQ0 32

// In main.c
// on Mem
void    *memcpy     (void *dest, void *src, uint count);
void    *memset     (void *dest, char val, uint count);
short   *memsetw    (short *dest, short val, uint count);
// on String
uint    strlen(char *str);

// In video.c
void    video_init  ();
void    cls         ();
void    putch       (char ch);
void    puts        (char *ch);
void    printf      (char *fmt, ...);

// In idt.c
struct idt_entry {
    ushort      base_lo;
    ushort      sel;
    uchar       always0;
    uchar       flags;
    ushort      base_hi;
} __attribute__((packed));


struct idt_desc {
    ushort      limit;
    uint        base;
} __attribute__((packed));     

struct regs {
    uint        gs, fs, es, ds;                            /* pushed the segs last */
    uint        edi, esi, ebp, esp, ebx, edx, ecx, eax;    /* pushed by 'pusha' */
    uint        int_no, err_code;                          /* our 'push byte #' and ecodes do this */
    uint        eip, cs, eflags, useresp, ss;              /* pushed by the processor automatically */ 
};

typedef void (*int_handler_t)(struct regs *r);

void    idt_init();

// on x86, just inline
// in & out
// on x86, just inline
char    inb     (short port);
void    outb    (short port, char data);
void    lidt    (struct idt_desc *idt_desc);

#endif
