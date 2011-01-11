#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <mm.h>

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
    uint pa, npa, *pte;

    pa = la2pa(la);
    // if it's the last one who did the write. 
    if (coremap[PPN(pa)]==1) {
        pte = find_pte(la);
        *pte &= ~PTE_W;
        flush_cr3(pgdir);
        return;
    }
    // one page, decrease the reference count of the page frame.
    coremap[PPN(pa)]--;
    npa = pgalloc();
    memcpy(npa, pa, 0x1000);
    pgattach(npa, la, PTE_P | PTE_U | PTE_W);
    flush_cr3(pgdir);
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

