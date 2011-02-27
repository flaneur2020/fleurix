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
    mm_init();          puts("* init mm\n");
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
        sti(); 
        swtch();
    }
}

char *argv[] = {"abc\n", "testt\n", NULL};

void init(){
    int fd;
    static char buf[255] = {0, };
    do_mount(rootdev, NULL);
    cu->p_iroot = cu->p_wdir;
    cu->p_iroot->i_count++;
    do_open("/dev/tty0", O_RDWR, 0); //stdin
    do_dup(0); // stdout
    do_dup(0); // stderr
    do_fcntl(0, F_SETFD, 0); // turn off FD_CLOEXEC
    do_fcntl(1, F_SETFD, 0);
    do_fcntl(2, F_SETFD, 0);
    do_exec("/bin/test_fork", argv);

    for(;;);
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
