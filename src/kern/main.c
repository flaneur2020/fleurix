#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>

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
    page_init();        puts("* init paging\n");
    buf_init();         puts("* init buf\n");
    hd_init();          puts("* init hd\n");
    timer_init();       puts("* init timer\n");
    sched_init();       puts("* init sched\n");
    // proc[0] arises now
    // mount root fs
    puts("* mouting root fs \n");
    do_mount(rootdev, NULL);
    cu->p_cdir = iget(rootdev, ROOTINO);
    unlk_ino(cu->p_cdir);

    puts("* enter user mode\n");
    umode_init();
    
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

void debug_mknod(){
    int r;

    r = do_mknod("/xxdir", S_IFDIR, 0);
    printf("%x\n", r);
    r = do_mknod("/xxdir/hello", S_IFDIR, 0);
}

/* TODO: just for debug right now.
 * we need two procs at least.
 * */
char buf[512] = {0, };
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

    debug_mknod();
    return 0;
}
