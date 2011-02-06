#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>
//
#include <page.h>
#include <vm.h>
//
#include <buf.h>
#include <conf.h>
#include <hd.h>
//
#include <tty.h>
//
#include <super.h>
#include <inode.h>
#include <file.h>

void init();

void main(){
    cls();
    gdt_init();         puts("* init gdt\n");
    idt_init();         puts("* init idt\n");
    vm_init();          puts("* init vm\n");
    //
    proc0_init();       puts("* init proc0\n");
    buf_init();         puts("* init buf\n");
    tty_init();         puts("* init tty\n");
    hd_init();          puts("* init hd\n");
    timer_init();       puts("* init timer\n");
    keybd_init();       puts("* init keybd\n");
    puts("\n--- fleurix ---\n\n");
    sti();
    //
    kspawn(&init);
    for(;;){
        swtch();
    }
}

void init(){
    //do_exec("");
    static char buf[128] = {0, };
    for(;;) {
        int i;
        printf("$ ");
        memset(buf, 0, 128);
        tty_read(&tty0, buf, 128);
        printf("  %s\n", buf);
        swtch();
    }
}

/*
 * only called once in proc[1], make some initilizations and
 * enter user mode via set tf->cs to USER_CS.
 * */
int sys_setup(struct trap *tf) {
    tf->cs = USER_CS;
    tf->ds = USER_DS;
    tf->ss = USER_DS;
}
