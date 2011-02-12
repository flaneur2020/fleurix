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
 * given a page directory, find the pte where the virtual address
 * lied in. If the 'creat' parameter is set, allocate a page as 
 * middle page table.
 * */
struct pte* find_pte(struct pte *pgd, uint vaddr, uint creat){
    struct pde *pde; 
    struct pte *pt;
    struct page *pg;

    pde = &pgd[PDX(vaddr)];
    if ((pde->pd_flag & PTE_P)==0) {
        if (creat==0) {
            panic("no pde");
            return NULL;
        }
        pg = pgalloc();
        pde->pd_flag = PTE_P | PTE_W;
        pde->pd_off = pg->pg_num;
        pt = (struct pte*)(pde->pd_off << 12);
        memset(pt, 0, PAGE);
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
    return 0;
}

/**/
int vm_free(){
}

/* Have a check of virtual memory area on getting a user space 
 * pointer, x86 do not raise page fault on ring0, so simulate
 * a write only access as what mmu does if nessary.
 * On size==0, check the memory area as a string.
 * */
int vm_verify(char* addr, uint size){
}

