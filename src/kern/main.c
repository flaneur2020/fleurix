#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <signal.h>
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

void kmain(){
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

void init() {
    struct inode *ip;

    cu->p_pgrp = 1;
    // mount rootfs
    do_mount(rootdev, NULL);
    ip = iget(rootdev, 1);
    cu->p_wdir = ip;
    cu->p_iroot = ip;
    ip->i_count += 2;
    iput(ip);
    // stdin, stdout, stderr
    if (do_open("/dev/tty0", O_RDWR, 0) < 0) {
        panic("bad /dev/tty0.\n");
    }
    do_dup(0); // stdout
    do_dup(0); // stderr
    do_fcntl(0, F_SETFD, 0); // turn off FD_CLOEXEC
    do_fcntl(1, F_SETFD, 0);
    do_fcntl(2, F_SETFD, 0);
    // enter user mode, never returns.
    do_exec("/bin/init", NULL);
    for(;;);
}
