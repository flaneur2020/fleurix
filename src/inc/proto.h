#ifndef PROTO_H
#define PROTO_H

#include <idt.h>
#include <inode.h>
#include <tty.h>
#include <vm.h>

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

// mp.c
void do_pgfault(struct trap *tf);

// timer.c
void    do_timer();

// kern/syscall.c
void    do_syscall();

void    ltr(uint n);
void    lldt(uint n);
void    ljmp(ushort seg, uint offset);

// kern/exec.c
int upush(uint *esp, char *buf, int len);
int upush_argv(uint *esp, char *path, char **argv);

// mm/vm.c
int vm_clone(struct vm *to);
struct pte* find_pte(uint vaddr, uint creat);
int vma_init(struct vma *vp, uint base, uint size, uint flag, struct inode *ip, uint ioff);

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
struct inode* namei_parent(char *path, char **name);

// fs/file.c 
struct file* falloc(int fd);

// chr/tty.c
int tty_read(struct tty *tp, char *buf, uint cnt);
int tty_write(struct tty *tp, char *buf, uint cnt);

/* --------------------------------------------------- */
/* panic */
#define panic(str)  \
    do {            \
        printf("\nPANIC: %s:%d: %s\n", __FILE__, __LINE__, str);\
        asm volatile("cli");\
        for(;;);\
    } while(0)

#define max(a, b) (((a)>(b))?(a):(b))
#define min(a, b) (((a)<(b))?(a):(b))

#endif
