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

/*
 * the proc[0]'s page directory, as initialized, it will map the top 4map virtual memory 
 * as physical memory, by the first middle page table.
 * */
struct pde pgd0[1024] __attribute__((aligned(4096)));

/* --------------------------------------------------------- */

/*
 * given the current proc's page directory, find the pte where 
 * the virtual address lied in. If the 'creat' parameter is set,
 * allocate a page as middle page table and always success.
 * */
struct pte* find_pte(uint vaddr, uint creat){
    struct pde *pde, *pgd; 
    struct pte *pt;
    struct page *pg;

    if (vaddr < KMEM_END) {
        panic("find_pte(): don't touch kernel's address space.");
    }
    pgd = cu->p_vm.vm_pgd;
    pde = &pgd[PDX(vaddr)];
    if ((pde->pd_flag & PTE_P)==0) {
        if (creat==0) {
            return NULL;
        }
        pg = pgalloc();
        pde->pd_flag = PTE_P | PTE_U | PTE_W;
        pde->pd_off = pg->pg_num;
        pt = (struct pte*)(pde->pd_off << 12);
        memset(pt, 0, PAGE);
        lpgd(pgd);
    }
    pt = (struct pte*)(pde->pd_off << 12);
    return &pt[PTX(vaddr)];
}

/* --------------------------------------------------------- */

/*
 * Clone the kernel's address space first. 
 * */
int vm_clone(struct vm *to, struct vm *from){
    int i;

    to->vm_pgd = (struct pde*)kmalloc(PAGE);
    // dup the kernel's address space first
    for (i=0; i<PMEM/(PAGE*1024); i++) {
        to->vm_pgd[i] = from->vm_pgd[i];
    }
    // 
    for (i=PMEM/(PAGE*1024); i<1024; i++) {
        to->vm_pgd[i] = from->vm_pgd[i];
        to->vm_pgd[i].pd_flag &= ~PTE_W;
    }
    return 0;
}

/**/
int vm_free(){
}

/* Have a check of virtual memory area on getting a user space 
 * pointer, on writing a write protected page, x86 do not raise 
 * a page fault in ring0, so simulate a write only access as 
 * what mmu does if nessary.
 * note: on verifying kernel memory, this routine will raise a 
 * panic.
 * */
int vm_verify(uint vaddr, uint size){
    struct pde *pgd;
    struct pte *pte;
    struct page *pg;
    uint offset[2], off, i;
    
    if (size>PAGE || size<=0) {
        panic("vm_verify(): bad size");
    }
    offset[0] = 0;
    offset[1] = size-1;
    for (i=0; i<2; i++) {
        off = offset[i];
        pte = find_pte(vaddr+off, 1);
        if ((pte->pt_flag & PTE_P)==0) {
            do_no_page(vaddr+off);
        }
        else if ((pte->pt_flag & PTE_W)==0) {
            do_wp_page(vaddr+off);
        }
    }
    return 0;
}

