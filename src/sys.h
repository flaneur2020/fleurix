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

#define IRQ0 32

// Eflags register
#define FL_CF           0x00000001      // Carry Flag
#define FL_PF           0x00000004      // Parity Flag
#define FL_AF           0x00000010      // Auxiliary carry Flag
#define FL_ZF           0x00000040      // Zero Flag
#define FL_SF           0x00000080      // Sign Flag
#define FL_TF           0x00000100      // Trap Flag
#define FL_IF           0x00000200      // Interrupt Enable
#define FL_DF           0x00000400      // Direction Flag
#define FL_OF           0x00000800      // Overflow Flag
#define FL_IOPL_MASK    0x00003000      // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000      //   IOPL == 0
#define FL_IOPL_1       0x00001000      //   IOPL == 1
#define FL_IOPL_2       0x00002000      //   IOPL == 2
#define FL_IOPL_3       0x00003000      //   IOPL == 3
#define FL_NT           0x00004000      // Nested Task
#define FL_RF           0x00010000      // Resume Flag
#define FL_VM           0x00020000      // Virtual 8086 mode
#define FL_AC           0x00040000      // Alignment Check
#define FL_VIF          0x00080000      // Virtual Interrupt Flag
#define FL_VIP          0x00100000      // Virtual Interrupt Pending
#define FL_ID           0x00200000      // ID flag

// Application segment type bits
#define STA_X       0x8     // Executable segment
#define STA_E       0x4     // Expand down (non-executable segments)
#define STA_C       0x4     // Conforming code segment (executable only)
#define STA_W       0x2     // Writeable (non-executable segments)
#define STA_R       0x2     // Readable (executable segments)
#define STA_A       0x1     // Accessed

// System segment type bits
#define STS_LDT     0x2     // Local Descriptor Table
#define STS_TG      0x5     // Task Gate / Coum Transmitions
#define STS_TA      0x9     // Available 32-bit TSS
#define STS_TB      0xB     // Busy 32-bit TSS
#define STS_CG      0xC     // 32-bit Call Gate
#define STS_IG      0xE     // 32-bit Interrupt Gate
#define STS_TG      0xF     // 32-bit Trap Gate


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
    uint        base_lo :16;        // low address
    uint        sel     :16;        // selector
    uint        always0 :8;         // reserved
    uint        type    :4;         // type(STS_{TG,IG32,TG32})
    uint        sys     :1;         // must be 0 (system)
    uint        dpl     :2;         // descriptor(meaning new) privilege level
    uint        p       :1;         // Present
    uint        base_hi :16;        // high address
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

void    idt_init();

// on x86, just inline
// in & out
// on x86, just inline
char    inb     (short port);
void    outb    (short port, char data);
void    lidt    (struct idt_desc *idt_desc);


// timer.c
void    do_timer();
#endif
