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
struct pde pgd0[1024] __attribute__((aligned(4096)));

/* --------------------------------------------------------- */

struct pte* find_pte(struct pte *pgd, uint vaddr){
    struct pde *pde; 
    struct pte *pt;

    pde = &pgd[PDX(vaddr)];
    if ((pde->pd_flag & PTE_P)==0) {
        panic("no pde");
        return NULL;
    }
    pt = (struct pte*)(pde->pd_off << 12);
    return &pt[PTX(vaddr)];
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

    // map the entire physical memory into the kernel's address space.
    for (pn=0; pn<PMEM/(PAGE*1024); pn++) {
        pgd0[pn].pd_off = pn << 10;
        pgd0[pn].pd_flag = PTE_PS | PTE_P | PTE_W; // note: set it 4mb via a PTE_S
    }
    //
    for (pn=PMEM/(PAGE*1024); pn<1024; pn++) {
        pgd0[pn].pd_flag &= ~PTE_P;
    }
    // init physical page allocator
    pm_init();
    // set fault handler
    set_hwint(0x0E, do_pgfault);
    // load page directory and enable the MMU.
    lpgd((uint)pgd0);
    mmu_enable();
}
