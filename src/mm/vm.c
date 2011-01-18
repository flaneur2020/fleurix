#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <page.h>
#include <vm.h>

void do_pgfault(struct trap *tf);

/*
 * vm.c
 *
 *
 * */

/*
 * the proc[0]'s page directory, as initialized, it will map the top 4map virtual memory 
 * as physical memory, by the first middle page table.
 * */
struct pte *pgdir0 = (struct pte*)0x0;
struct pte *pmd0 = (struct pte*)(0x0+PAGE);

/* --------------------------------------------------------- */

struct pte* find_pte(struct pte *pgdir, uint vaddr){
    struct pte pde, *pmd;

    pde = pgdir[PDX(vaddr)];
    if ((pde.pt_flag & PTE_P)==0) {
        panic("no pde");
        return NULL;
    }
    pmd = (struct pte*)(pde.pt_num << 12);
    return &pmd[PTX(vaddr)];
}

/* --------------------------------------------------------- */

/*
 * Clone the kernel's address space first. 
 * */
int vm_clone(struct vm *to, struct vm *from){
}

/**/
int vm_free(){
}

int vm_new(){
}

/* Have a check of virtual memory area on getting a user space 
 * pointer, x86 do not raise page fault on ring0, so simulate
 * a write only access as what mmu does if nessary.
 * On size==0, check the memory area as a string.
 * */
int vm_verify(char* addr, uint size){
}

/* --------------------------------------------------- */

/* Map the top 4mb virtual memory as physical memory. Initiliaze
 * the page-level allocator, set the fault handler and misc.
 * */
void vm_init(){
    int pn;

    for (pn=0; pn<1024; pn++) {
        pmd0[pn].pt_flag = PTE_P | PTE_W;
        pmd0[pn].pt_num = pn;
    }
    pgdir0[0].pt_num = PPN(pmd0);
    //
    for (pn=1; pn<1024; pn++) {
        pgdir0[pn].pt_flag &= ~PTE_P;
    }
    // init physical page allocator
    pm_init();
    // set fault handler
    set_hwint(0x0E, do_pgfault);
    // load page directory and enable the MMU.
    lpgdir((uint)pgdir0);
    mmu_enable();
}
