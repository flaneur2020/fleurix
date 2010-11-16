#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>
#include <unistd.h>

#include <buf.h>
#include <conf.h>
#include <hd.h>

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
void sys_setup(struct trap *tf){
    int dev = DEVNO(0, 0);
    struct buf *bp= bread(dev, 2);
    char* data = bp->b_addr;
    int i;
    for(i=0; i<100; i++){
        printf("%d", data[i]);
    }
    printf("ok\n");
    printf("second read\n");
    data = bp->b_addr;
    for(i=0; i<100; i++){
        printf("%d", data[i]);
    }
}

