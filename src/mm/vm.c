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
#include <a.out.h>

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
            // increase the reference count of inode
            if (vp->v_ino) {
                vp->v_ino->i_count++;
            }
            to->vm_area[i] = *vp;
        }
    }
    // copy pages tables, with PTE_W turned off.
    pt_copy(to->vm_pgd, cu->p_vm.vm_pgd);
    return 0;
}

/* free all the pages used in this process, deallocate all the page tables, 
 * this routine is called on freeing one task in do_exit(), or overlapping 
 * a proc's address space on do_exec() is being called. 
 * note: this routine will *NOT* free the pgd.
 * */
int vm_clear(struct vm *vm){
    struct vma *vp;
    int i;

    for (i=0; i<NVMA; i++) {
        vp = &vm->vm_area[i];
        if (vp->v_flag != 0) {
            if (vp->v_ino)
                iput(vp->v_ino);
            vp->v_ino = NULL;
            vp->v_flag = 0;
            vp->v_base = 0;
            vp->v_size = 0;
        }
    }
    // free the page tables. (but pgd is not freed yet.)
    pt_free(vm->vm_pgd);
    return 0;
}

/* renew a adress space, called on do_exec() */
int vm_renew(struct vm *vm, struct ahead *ah, struct inode *ip){
    uint base, text, data, bss, heap;

    base = ah->a_entry - sizeof(struct ahead); // note: keep alignment
    text = ah->a_entry - sizeof(struct ahead);
    data = text + ah->a_tsize;
    bss  = data + ah->a_dsize;
    heap = bss  + ah->a_bsize; 
    //
    pgd_init(vm->vm_pgd);
    vm->vm_entry = ah->a_entry;
    vma_init(&(vm->vm_text),  text,  ah->a_tsize, VMA_MMAP | VMA_RDONLY | VMA_PRIVATE, ip, text-base);
    vma_init(&(vm->vm_data),  data,  ah->a_dsize, VMA_MMAP | VMA_PRIVATE, ip, data-base);
    vma_init(&(vm->vm_bss),   bss,   ah->a_bsize, VMA_ZERO | VMA_PRIVATE, NULL, 0);
    vma_init(&(vm->vm_heap),  heap,  PAGE,        VMA_ZERO | VMA_PRIVATE, NULL, 0);
    vma_init(&(vm->vm_stack), VM_STACK, PAGE,     VMA_STACK | VMA_ZERO | VMA_PRIVATE, NULL, 0);
    return 0;
}

/* Have a check of virtual memory area on getting a user space 
 * pointer, on writing a write protected page, x86 do not raise 
 * a page fault in ring0, so simulate a write only access as 
 * what mmu does if nessary.
 *
 * note:  
 *   be aware that do NOT touch the memory in the second argument
 *   like `vm_verify(path, strlen(path) + 1)`, which is a bug
 *
 * note2: 
 *   use this routine only before reading and writing the 
 *   user memory.
 * */
int vm_verify(uint vaddr, uint size){
    struct pte *pte;
    uint page;
    
    if (vaddr<KMEM_END || size<0) {
        return -1;
    }
    // TODO: special case on checking string.
    for (page=PG_ADDR(vaddr); page<=PG_ADDR(vaddr+size-1); page+=PAGE) {
        pte = find_pte(cu->p_vm.vm_pgd, page, 1);
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
    return 0;
}

/* -------------------------------------------------- */

void dump_vm(struct vm *vp){
    printk("vm: %x\n", vp);
    printk("vm: ino: %x\n", vp->vm_text.v_ino);
    dump_inode(vp->vm_text.v_ino);
}
