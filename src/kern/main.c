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
#include <fs.h>

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
        while(1) asm volatile("int $0x80"::"a"(1),"b"(0));
    }
    else if(fork()){
        while(1) asm volatile("int $0x80"::"a"(1),"b"(1));
    }
    else {
        while(1) asm volatile("int $0x80"::"a"(1),"b"(2));
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
void sys_setup(struct trap *tf) {
    int dev = DEVNO(1, 0);
    struct buf *bp;
    int i, ino, nr;
    struct super *sp;
    struct inode *ip, *rip;
    char *str, *path;

    //
    do_mount(rootdev, NULL);

    // debug read_inode();
    //
    ip = iget(rootdev, 2);
    read_inode(ip);
    dump_inode(ip);
    panic("~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

    ip = iget(rootdev, 2);
    dump_inode(ip);
    iput(ip);

    // debug namei
    path = "/dir/d";
    rip = do_namei(path);
    if (rip==NULL) {
        printf("%s : inode not found", path);
    }
    else {
        printf("path = %s\n ino = %d\n", path, rip->i_num);
    }
    iput(ip);
    unlock_super(sp);
}

