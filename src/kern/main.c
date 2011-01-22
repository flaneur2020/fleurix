#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>

#include <page.h>
#include <vm.h>

#include <buf.h>
#include <conf.h>
#include <hd.h>

#include <super.h>
#include <inode.h>
#include <file.h>

void main(){
    tty_init();         puts("* init tty\n");
    gdt_init();         puts("* init gdt\n");
    idt_init();         puts("* init idt\n");
    vm_init();          puts("* init vm\n");
    proc0_init();       puts("* init sched\n");

    /* ------------------------------------------------- */
    int i, *r;
    struct page *pp;
    for (i=0; i<0x1000; i++){
        r = (int *)kmalloc(32);
        printf("kmalloc(): %x \n", r); 
        *r = 1; // writing to 0x11000 may raise a #GPF
    }

    for(;;);
    panic("trying a new vm now...");

    buf_init();         puts("* init buf\n");
    hd_init();          puts("* init hd\n");
    timer_init();       puts("* init timer\n");
    // proc[0] arises now
    // mount root fs
    puts("* mouting root fs \n");
    do_mount(rootdev, NULL);
    cu->p_cdir = iget(rootdev, ROOTINO);
    unlk_ino(cu->p_cdir);

    puts("* enter user mode\n");
    _umode();
    
    /*
    if (fork()){
        while(1); // do nothing , don't pollute the stack.
    }
    else if(fork()){
        debug(1);
        while(1) debug(1);
    }
    else {
        debug(2);
        while(1) debug(2);
    }
    */
    if(fork()==0){
        setup(); // note: setup is inlined, or something unhappy may occurs.
        //access("abc",123);
    }

    for(;;);
}

char buf[512] = {0, };
void debug_open(){
    int fd, r, fd2;

    fd = do_open("/about.txt", O_RDONLY, 0);
    if (fd < 0) {
        panic("bad open");
    }
    memset(buf, 0, 20); do_read(fd, buf, 20); buf[20] = '\0'; printf("%s\n", buf);
    fd2 = do_dup(fd);
    do_close(fd);
    do_close(fd2);
}

/* TODO: just for debug right now.
 * we need two procs at least.
 * */
int sys_setup(struct trap *tf) {
    printf("...\n");
    int dev = DEVNO(1, 0);
    struct buf *bp;
    int i, ino, fd, nr, r;
    struct super *sp;
    struct inode *ip, *rip;
    char *str, *path;

    // mount the root dev and set the current workint dirctory

    /*--------------------*/

    debug_open();
    return 0;
}
