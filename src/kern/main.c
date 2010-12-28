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
char buf[512] = {0, };
void sys_setup(struct trap *tf) {
    int dev = DEVNO(1, 0);
    struct buf *bp;
    int i, ino, fd, nr, r;
    struct super *sp;
    struct inode *ip, *rip;
    char *str, *path;

    // mount the root dev and set the current workint dirctory

    /*--------------------*/

    ino = ialloc(rootdev);
    ip = iget(rootdev, ino);
    ip->i_nlinks = 1;
    ip->i_mode = S_IFREG | 0777;
    ip->i_time = time();
    ip->i_gid = 0xe8;
    iupdate(ip);
    dump_inode(ip);
    iput(ip);
    //
    ip = namei("/12345678901234567890.txt", 1);
    if (ip==NULL){
        panic("bad ino");
    }
    iupdate(ip);
    iput(ip);
    //dump_inode(ip);
    unlk_ino(ip);
    if (ip==NULL) {
        panic("damn");
    }
}
