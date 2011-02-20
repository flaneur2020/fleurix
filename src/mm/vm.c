#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
//
#include <page.h>
#include <vm.h>
//
#include <buf.h>
#include <conf.h>
#include <hd.h>
//
#include <super.h>
#include <inode.h>
#include <file.h>

/*
 * vm.c
 *
 *
 * */

/* --------------------------------------------------------- */

/*
 * Clone the kernel's address space first. 
 * */
int vm_clone(struct vm *to, struct vm *from){
    int i;
    struct vma *vp;

    to->vm_pgd = (struct pde*)kmalloc(PAGE);
    // clone kernel's address space
    pgd_init(to->vm_pgd);
    // clone user's
    for (i=0; i<NVMA; i++) {
        vp = &(cu->p_vm.vm_area[i]);
        pgd_copy(to->vm_pgd, vp->v_base, vp->v_size, PTE_P|PTE_U); // turn off both's PTE_W
    }
    return 0;
}

/* */
int vm_free(struct vm *vm){
}

/* Have a check of virtual memory area on getting a user space 
 * pointer, on writing a write protected page, x86 do not raise 
 * a page fault in ring0, so simulate a write only access as 
 * what mmu does if nessary.
 * note: do not verify kernel memory.
 * */
int vm_verify(uint vaddr, uint size){
    struct pde *pgd;
    struct pte *pte;
    struct page *pg;
    uint page;
    
    if (vaddr<KMEM_END || size<=0) {
        return -1;
    }
    for (page=PG_ADDR(vaddr); page<=PG_ADDR(vaddr+size-1); page+=PAGE) {
        pte = find_pte(page, 1);
        if ((pte->pt_flag & PTE_P)==0) {
            do_no_page(page);
        }
        else if ((pte->pt_flag & PTE_W)==0) {
            do_wp_page(page);
        }
    }
    return 0;
}

