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

/*
 * NOTE: share the memory of proc[0] may raise some wired bugs.
 * */

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
    puts("* init user mode\n");
    umode_init();

    /*
    if (fork()){
        while(1); // do nothing , don't pollute the stack.
    }
    else if(fork()){
        putn(1);
        while(1) putn(1);
    }
    else {
        putn(2);
        while(1) putn(2);
    }
    for(;;);
    */
    // in proc1
    if(fork()==0){
        // setup
        asm volatile("int $0x80"::"a"(0));
    }

    for(;;);
}

/* TODO: just for debug right now.
 * we need two procs at least.
 * */
char buf[256] = {0, };
void sys_setup(struct trap *tf) {
    int dev = DEVNO(1, 0);
    struct buf *bp;
    int i, ino, nr;
    struct super *sp;
    struct inode *ip, *rip;
    char *str, *path;

    // mount the root dev and set the current workint dirctory
    do_mount(rootdev, NULL);
    ip = iget(rootdev, ROOTINO);
    current->p_cdir = ip; 
    unlock_inode(ip);

    ip = do_namei("/test.txt");
    readi(ip, buf, 12400, 20);
    printf("%s", buf);
    iput(ip);
    int fd = do_open("/test.txt", O_RDONLY);
    int r=0;
    while((r=do_read(fd, buf, 50)) > 0){
        buf[50] = '\0';
        struct file *fp = current->p_ofile[fd];
        //printf("%s", buf);
    }
    printf("...\n");
    unlk_sp(sp);
}

