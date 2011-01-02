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

// drv/bio.c
int     nodev();
int     nulldev();

// fs/inode.c
struct inode* iget(ushort dev, uint num);
void iput(struct inode *ip);
int bmap(struct inode *ip, ushort nr, uchar creat);
int iload(struct inode *ip);
void iupdate(struct inode *ip);

// fs/alloc.c
int balloc(ushort dev);
int bfree(ushort dev, uint nr);
int bzero(ushort dev, uint bn);
int ialloc(ushort dev);
void ifree(ushort dev, uint ino);

// fs/namei.c
struct inode* namei(char *path, uchar creat);

// fs/file.c 
struct file* falloc(int fd);

/* --------------------------------------------------- */
/* panic */
#define panic(str)  \
    do {            \
        printf("\n%s:%s():%d: \nPANIC: %s\n", __FILE__, __func__, __LINE__, str);\
        asm volatile("cli");\
        for(;;);\
    } while(0)

#define max(a, b) (((a)>(b))?(a):(b))
#define min(a, b) (((a)<(b))?(a):(b))

