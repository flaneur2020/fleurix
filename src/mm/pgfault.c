#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <page.h>
#include <vm.h>

/*
 * on swap...
 * */
void do_no_page(uint la, uint err){
}

/* 
 * the handler of the read-only protection. here is the 'copy' action
 * of the implemtion of copy-on-write.
 * If this one is the last one who did the write, just mark the page as
 * write-able.
 * else allocate one page and associate inside the page table. 
 * */
void do_wp_page(uint la, uint err){
}

/*
 * the common handler of all page faults, as a dispatcher.
 * */
void do_pgfault(struct trap *tf){
    uint cr2;

    asm volatile("movl %%cr2, %0":"=a"(cr2));
    // write protection raised prior than valid.
    if (tf->err_code & PFE_W) {
        do_wp_page(cr2, tf->err_code);
        return;
    }
    //debug_regs(tf);
    if (tf->err_code & PFE_U) {
        panic("user bad mem access.");
    }
    printf("page fault: %x \nerr_code %x\n", cr2, tf->err_code);
    panic("~");
}

