#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <page.h>
#include <vm.h>

/*
 * the handler of a page fault raised on PTE_P is off.
 * */
void do_no_page(uint vaddr){
    struct vm *vm;
    struct vma *vma;
    struct page *pg;
    uint bn;
    char *buf;

    vm = &cu->p_vm;
    // if this page lies on the edge of user stack, grows the stack.
    if (vm->vm_stack.v_base - vaddr < PAGE) {
        vm->vm_stack.v_base -= PAGE;
        vm->vm_stack.v_size += PAGE;
        pg = pgalloc();
        pgattach(vm->vm_pgd, PTE_ADDR(vaddr), pg, PTE_U|PTE_W|PTE_P);
        return;
    }
    // else
    vma = find_vma(vaddr);
    if (vma==NULL) {
        // TODO: segmentation fault.
        return;
    }
    // demand zero
    if (vma->v_flag & VMA_ZERO) {
        pg = pgalloc();
        pgattach(vm->vm_pgd, PTE_ADDR(vaddr), pg, PTE_U|PTE_W|PTE_P);
        memset(PG_ADDR(pg), 0, PAGE);
        return;
    }
    // demand file
    if (vma->v_flag & VMA_MMAP) {
        pg = pgalloc();
        pgattach(vm->vm_pgd, PTE_ADDR(vaddr), pg, PTE_U|PTE_W|PTE_P);
        // fill this new-allocated page
        buf = PG_ADDR(pg);
        for(bn=0; bn<PAGE/BLK; bn++) {
            readi(vma->v_ino, buf, bn*BLK, BLK);
        }
        return;
    }
    // segmentation fault
    panic("do_no_page()");
}

/* 
 * the handler of the read-only protection. here is the 'copy' action
 * of the implemtion of copy-on-write.
 * If this one is the last one who did the write, just mark the page as
 * write-able.
 * else allocate one page and associate inside the page table. 
 * */
void do_wp_page(uint vaddr){
    panic("do_wp_page();");
}

/*
 * the common handler of all page faults, as a dispatcher.
 * */
void do_pgfault(struct trap *tf){
    uint cr2;

    asm volatile("movl %%cr2, %0":"=a"(cr2));
    // invalid page
    if (tf->err_code & PFE_P) {
        do_no_page(cr2);
        return;
    }
    // write procted page
    if (tf->err_code & PFE_W) {
        do_wp_page(cr2);
        return;
    }
    // TODO: raise a signal here, hence segmention fault.
    if (tf->err_code & PFE_U) {
        panic("user bad mem access.");
    }
    printf("page fault: %x \nerr_code %x\n", cr2, tf->err_code);
    panic("~");
}

