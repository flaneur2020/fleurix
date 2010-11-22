#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>

#include <buf.h>
#include <conf.h>
#include <hd.h>

#include <super.h>

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
    int dev = DEVNO(0, 0);
    struct buf *bp;
    struct super *sp;

    mount_root(dev);
    sp = get_super(dev);
    printf("max_inode:%d\n", sp->s_max_inode);
    printf("max_zone:%d\n", sp->s_max_zone);
    printf("s_nimap_blk:%d\n", sp->s_nimap_blk);
    printf("s_nzmap_blk:%d\n", sp->s_nzmap_blk);
    printf("zone0:%d\n", sp->s_zone0);
    printf("log_zone:%d\n", sp->s_log_zone);
    printf("s_max_size:%d\n", sp->s_max_size);
    printf("s_nzone:%d\n", sp->s_nzone);
    printf("magic:%x\n", sp->s_magic);
}

