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
 * */

/* --------------------------------------------------------- */

/*
 * Clone the kernel's address space first, the only way on
 * initializing a struct vm.
 * */
int vm_clone(struct vm *to){
    int i;
    struct vma *vp;

    to->vm_pgd = (struct pde*)kmalloc(PAGE);
    // clone kernel's address space
    pgd_init(to->vm_pgd);
    // clone user's, aka for each VMA.
    for (i=0; i<NVMA; i++) {
        vp = &(cu->p_vm.vm_area[i]);
        if (vp->v_flag != 0) {
            to->vm_area[i] = *vp;
            pt_copy(to->vm_pgd, vp->v_base, vp->v_size, PTE_P|PTE_U); // turn off both's PTE_W
        }
    }
    return 0;
}

/* free all the pages used in this process, deallocate all the 
 * page tables, 
 * note: this routin will *NOT* free the pgd.
 * */
int vm_free(struct vm *vm){
    struct vma *vp;
    int i;

    for (i=0; i<NVMA; i++) {
        vp = &vm->vm_area[i];
        if (vp->v_flag != 0) {
            pt_free(vm->vm_pgd, vp->v_base, vp->v_size);
            vp->v_flag = 0;
            vp->v_base = 0;
            vp->v_size = 0;
            if (vp->v_ino)
                iput(vp->v_ino);
        }
    }
    return 0;
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

/* ------------------------------------------------------------------------- */

/*
 * find the vma where the vaddr lied in, mainly called on do_no_page() or do_wp_page().
 * returns NULL on fail.
 * */
struct vma* find_vma(uint addr){
    struct vma* vma;
    struct vma* vp;
    //
    vma = cu->p_vm.vm_area;
    for (vp=&vma[0]; vp<&vma[NVMA]; vp++) {
        if (addr >= vp->v_base && addr < vp->v_base+vp->v_size) {
            return vp;
        }
    }
    return NULL;
}

/* initialize a VMA.
 * note that each vma is associated with one inode. */
int vma_init(struct vma *vp, uint base, uint size, uint flag, struct inode *ip, uint ioff){
    vp->v_flag = flag;
    vp->v_base = base;
    vp->v_size = size;
    vp->v_ioff = ioff;
    if (ip) {
        ip->i_count++;
        vp->v_ino = ip;
    }
}

