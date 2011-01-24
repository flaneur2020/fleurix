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
    buf_init();         puts("* init buf\n");
    hd_init();          puts("* init hd\n");
    timer_init();       puts("* init timer\n");
    sti(); // timer enabled here, in fact

    if(fork()==0){
        printf("proc 1\n");
        // exec();
        for(;;);
    }

    for(;;) {
        printf("proc 0\n");
        swtch();
        for(;;);
    }
}

/*
 * only called once in proc[1], make some initilizations and
 * enter user mode via set tf->cs.
 * */
int sys_setup(struct trap *tf) {
    tf->cs = USER_CS;
    tf->ds = USER_DS;
    tf->ss = USER_DS;
}
