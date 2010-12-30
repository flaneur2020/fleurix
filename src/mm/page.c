#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

uint la2pa(uint la);
uint alloc_page();
uint* find_pte(uint la);

/*
 * page.c
 *
 * this file indicated the codes on the initialization of the paging system and the
 * implemtion of copy-on-write, maybe demand paging is the next.
 * 
 * Fleurix takes an seg & page memory model the same as linux0.11 and inherit the 16mb
 * physical memory limit. Total 4Gb linear space address, and each process has got 64mb 
 * virtual address space, which seperated by segments lied inside its LDT. 
 *
 * This file is heavily associated with fork(). In the routine copy_mem_to(), it will copy 
 * the parent's page tables to the children, and mark each read-only, take an eye on the 
 * fact that *** the parent's page table is also marked read-only *** in routine copy_vm().
 * Hence each modification will raise an page fault, so do_wp_page() handled this and copy 
 * one page or just un-wp it(if it's the last one who does modify something).
 *
 * */

/*
 * the ONLY page directory, which indicates the linear address space of 4GB.
 * each proc have a address space of 64MB, which starts at pid*64mb. limited 
 * by each's LDT.
 * */
uint *pgdir = (uint *) 0x00000;

/*
 * the map for page frames. Each physical page is associated with one reference
 * count, and it's free on 0. Only 0 can be allocated via alloc_page().  
 * Reference count is increased on a fork.
 *
 * note: the kernel pages(0~LO_MEM) are initialized as 100 (in page_init()) to 
 * prevent any allocation.
 * */
uchar coremap[NPAGE] = {0, };

/**************************************************************/

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
    npa = alloc_page();
    memcpy(npa, pa, 0x1000);
    put_page(npa, la, PTE_P | PTE_U | PTE_W);
    flush_cr3(pgdir);
}

/*
 * the common handler of all page faults, as a dispatcher.
 * */
void do_page_fault(struct trap *tf){
    uint cr2;
    asm volatile("movl %%cr2, %0":"=a"(cr2));
    // write protection raised prior than valid.
    if (tf->err_code & PFE_W) {
        do_wp_page(cr2, tf->err_code);
    }
    //debug_regs(tf);
}

/**************************************************************/

/* 
 * allocate an free physical page. 
 * tranverse coremap, if 0, set it 1 and return the address
 * */ 
uint alloc_page(){
    int i;
    for(i=0;i<NPAGE; i++){
        if(coremap[i]==0){
            coremap[i]++;
            return i*0x1000;
        }
    }
    panic("no availible page.\n");
    return 0;
}

/*
 * free a physical page. decrease the target inside coremap. 
 */
int free_page(uint addr){
    int n;
    n = addr/0x1000;
    if (n<NKPAGE || n>NPAGE) {
        panic("error page.");
    }
    if(coremap[n]==0){
        return 0;
    }
    coremap[n]--;
    return n;
}

/****************************************************************/

/*
 * map a linear address to physical address.  
 * */
int put_page(uint pa, uint la, uint flag){
    uint pde = pgdir[PDX(la)];
    if (!(pde & PTE_P)){
        pde = alloc_page();
        if (pde==0){
            panic("no availible frame");
        }
        // it's a pde, don't get confused.
        pgdir[PDX(la)] = pde | PTE_P | PTE_W | PTE_U;
    }
    uint *ptab = (uint *)PTE_ADDR(pde);
    ptab[PTX(la)] = pa | flag;
    coremap[pa/0x1000]++;
    return 0;
}

/*
 * copy page tables, as a helper of copy_proc().
 * this also mark the parent proc's page tables as read only.
 * note: *important*, but you should never mark the proc[0]'s page
 * as read only.
 * note2: src, dst, and limit are deserved multiple of 0x1000
 * */
int copy_vm(uint dst, uint src, uint limit){
    uint off, la, pa, *pte;
    for(off=0; off<=limit; off+=0x1000){
        // find and mark the parent's page as read only.
        // but not do this on proc0
        if (src >= LO_MEM) {
            pte = find_pte(src+off);
            *pte &= ~PTE_W; 
        }
        // increase the children's reference count.
        pa = la2pa(src+off);
        coremap[PPN(pa)]++;
        put_page(pa, dst+off, PTE_P | PTE_U);
    }
    flush_cr3(pgdir);
    return 0;
}

/**************************************************************/

/* get the pointer to a pte. */
uint* find_pte(uint la){
    uint pde, *ptab, pte;
    pde = pgdir[PDX(la)];
    if (!(pde & PTE_P)) {
        printf("%x: ", la);
        panic("invalid pde\n");
    }
    ptab = (uint *)PTE_ADDR(pde);
    return &ptab[PTX(la)];
}

/* translate a linear address to physical address
 */
uint la2pa(uint la){
    uint pte = *find_pte(la);
    if(!(pte & PTE_P)){
        panic("invalid pte\n");
    }
    return PTE_ADDR(pte) + POFF(la);
}

uint va2la(uint va){
    if (va > PROCSIZ) {
        panic("bad virtual address");
    }
    return cu->p_pid*PROCSIZ + va;
}

/***************************************************************/

void flush_cr3(uint addr){
    asm volatile("mov %%eax, %%cr3":: "a"(addr));
}

void page_enable(){
    uint cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // set the paging bit.
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void page_init(){
    // address of a page directory have to make aligned to multiple of 4kb
    // the page table comes right after the page directory, 0x1000 == 1024 * 4
	uint *ptab;

	// map the top 16MB of memory
    //
    // FOUR page table, 16kb
    // fill the top FOUR entry of the page directory
    // attribute set to: supervisor level, read/write, present(011 in binary)
	uint i, j;
	uint addr = 0; 
    for(i=0; i<4; i++){
        ptab = 0x1000 + 0x1000*i;
        pgdir[i] = (uint)ptab | PTE_P | PTE_W | PTE_U;
        for(j=0; j<1024; j++) {
            ptab[j] = addr | PTE_P | PTE_W | PTE_U; 
            addr += 0x1000; // 4096 = 4kb
        }
    }

	// fill the rest of the page directory
    // attribute set to: supervisor level, read/write, not present(010 in binary)
	for(i=4; i<1024; i++) {
		pgdir[i] = 0 | PTE_W | PTE_U; 
	};

    // init coremap
    for(i=0; i<NKPAGE; i++) {
        coremap[i] = 100;
    }

    // int handler
    set_hwint(0x0E, do_page_fault);

    // write page directory to cr3 and enable PE on cr0
    flush_cr3(pgdir);
    page_enable();
}
