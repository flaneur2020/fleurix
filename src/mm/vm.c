#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <mm.h>

uint la2pa(uint la);
uint pgalloc();
uint* find_pte(uint la);
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

struct pte* find_pte(uint vaddr){
    struct pte *pgdir;
    struct pte *pmd;

    pgdir = cu->p_vm->vm_pgdir;
    pmd = pgdir[PDX(vaddr)];
    if ((pmd & PTE_P)==0) {
        panic("no pmd");
        return NULL;
    }
    return &pmd[PTX(vaddr)];
}

/* --------------------------------------------------------- */

/*
 * Clone the kernel's address space first. 
 * */
int vm_clone(struct pte *pdt, struct pte *pdf){
}

/**/
int vm_free(){
}

int vm_new(){
}

void vm_init(){
}

/* --------------------------------------------------- */

/**/
void vm_init(){
    int pn;

    for (pn=0; pn<1024; pn++) {
        pmd0[pn].pt_flag = PTE_P | PTE_W;
        pmd0[pn].pt_num = pn;
    }
    pgdir0[0].pt_num = PPN(pde0);
    // init physical page allocator
    pm_init();
    // set fault handler
    set_hwint(0x0E, do_pgfault);
    // load page directory and enable the MMU.
    lpgdir((uint)pgdir0);
    mmu_enable();
}
