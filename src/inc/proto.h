/** */ 
// tty.c
void    tty_init();
void    cls();
void    putch(char ch);
void    puts(char *ch);
void    printf(char *fmt, ...);

// trap.c
void    idt_init();

// seg.c
uint    get_seg_limit(struct seg_desc *seg);
extern  struct seg_desc     gdt[];

// page.c
uint    palloc();
int     pfree(uint addr);


// timer.c
void    do_timer();

// syscall.c
void    do_syscall();

void    ltr(uint n);
void    lldt(uint n);
void    ljmp(ushort seg, uint offset);

// bio.c
int     nodev();
int     nulldev();

/* panic */
#define panic(str)  \
    do {            \
        printf("\nPANIC %s:%d: %s\n", __FILE__, __LINE__, str);\
        asm volatile("cli");\
        for(;;);\
    } while(0)

#define max(a, b) (((a)>(b))?(a):(b))
#define min(a, b) (((a)<(b))?(a):(b))

