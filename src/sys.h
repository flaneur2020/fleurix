/**
 *
 * just misc right now
 *
 * */ 

typedef unsigned int    uint;
typedef unsigned int    size_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;

#define KERN_CS 0x08
#define KERN_DS 0x10

// In main.c
// on Mem
void    *memcpy     (void *dest, void *src, size_t count);
void    *memset     (void *dest, char val, size_t count);
short   *memsetw    (short *dest, short val, size_t count);
// on String
size_t strlen(char *str);

// In video.c
void    init_video  ();
void    cls         ();
void    putch       (char ch);
void    puts        (char *ch);
void    printf      (char *fmt, ...);

// In idt.c
struct idt_entry {
    uint16_t    base_lo;
    uint16_t    sel;
    uint8_t     always0;
    uint8_t     flags;
    uint16_t    base_hi;
} __attribute__((packed));

#define STS

struct idt_desc {
    uint16_t    limit;
    uint32_t    base;
} __attribute__((packed));     

struct regs {
    uint gs, fs, es, ds;                            /* pushed the segs last */
    uint edi, esi, ebp, esp, ebx, edx, ecx, eax;    /* pushed by 'pusha' */
    uint int_no, err_code;                          /* our 'push byte #' and ecodes do this */
    uint eip, cs, eflags, useresp, ss;              /* pushed by the processor automatically */ 
};

void    init_idt();

// on x86, just inline
// in & out
// on x86, just inline
char    inb     (short port);
void    outb    (short port, char data);
void    lidt    (struct idt_desc idt_desc);

