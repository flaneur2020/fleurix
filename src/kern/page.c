#include <param.h>
#include <x86.h>
#include <proto.h>

uint la2pa(uint la);
uint alloc_page();

/*
 * the ONLY page directory, 
 * which indicates the linear address space of 4GB
 * each proc have a address space of 64MB, which starts at pid*64mb
 * */
uint *pgdir = (uint *) 0x00000;

uchar coremap[NPAGE] = {0, };

/**************************************************************/

void do_no_page(uint la, uint err){
}

/* the handler of write-only protection.
 * here is the 'write' of the so called copy-on-write.
 * TODO: 
 * */
void do_wp_page(uint la, uint err){
    uint pa, npa;
    pa = la2pa(la);
    npa = alloc_page();
    memcpy(pa, npa, 0x1000);
    put_page(la, npa, PTE_P | PTE_U | PTE_W);
}

void do_page_fault(struct trap *tf){
    uint cr2;
    asm volatile("movl %%cr2, %0":"=a"(cr2));
    // write protection raised prior than valid.
    if (tf->err_code & PFE_W) {
        //do_wp_page(cr2, tf->err_code);
    }
    debug_regs(tf);
    for(;;);
}

/**************************************************************/

/* 
 * allocate an free physical page. 
 * tranverse coremap, if 0, set it 1 and return the address
 * */ 
uint alloc_page(){
    int i;
    for(i=0; i<NPAGE; i++){
        if(coremap[i]==0){
            coremap[i] = 1;
            return LO_MEM + i*0x1000;
        }
    }
    panic("no availible page.\n");
    return 0;
}

/*
 * free a physical page. decrease the target inside coremap. 
 */
int free_page(uint addr){
    int i;

    i = (addr-LO_MEM)/0x1000;
    if (i<0 || i>NPAGE) {
        panic("error page.");
    }
    if(coremap[i]==0){
        return 0;
    }
    coremap[i]--;
    return i;
}

/****************************************************************/

/*
 * map a linear address to physical address.  
 * */
int put_page(uint la, uint pa, uint flag){
    uint pde = pgdir[PDX(la)];
    if (!(pde & PTE_P)){
        pde = alloc_page();
        if (pde==0){
            panic("no availible frame");
        }
        // it's a pde
        pgdir[PDX(la)] = pde | PTE_P | PTE_W | PTE_U;
    }
    uint *ptab = (uint *)PTE_ADDR(pde);
    ptab[PTX(la)] = pa | flag;
    coremap[pa/0x1000]++;
    return 0;
}

/*
 * copy page tables, as a helper of copy_proc()
 * it do NOT copy the data inside a frame, just remap it
 * note: src, dst, and limit are deserved multiple of 0x1000
 *
 * TODO: and mark it READONLY.  note: the wired syscall bug may from this. 
 * */
int copy_ptab(uint src, uint dst, uint limit){
    //printf("copy_ptab(): src=%x, dst=%x, limit=%x\n", src, dst, limit);
    uint off, la, pa;
    for(off=0; off<=limit; off+=0x1000){
        pa = la2pa(src+off);
        put_page(dst+off, pa, PTE_P | PTE_W | PTE_U);
    }
    flush_cr3(pgdir);
    return 0;
}

/**************************************************************/

struct pte* find_pte(uint la){
}

/* translate a linear address to physical address
 */
uint la2pa(uint la){
    uint pde = pgdir[PDX(la)];
    if(!(pde & PTE_P)){
        printf("%x: ", la);
        panic("invalid pde\n");
    }
    uint *ptab = (uint *)PTE_ADDR(pde);
    uint pte = ptab[PTX(la)];
    if(!(pte & PTE_P)){
        panic("invalid pte\n");
    }
    return PTE_ADDR(pte) + POFF(la);
}

/***************************************************************/

void flush_cr3(uint addr){
    asm volatile("mov %%eax, %%cr3":: "a"(addr));
}

void page_enable(){
    uint cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
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
            addr += 4096; // 4096 = 4kb
        }
    }

	// fill the rest of the page directory
    // attribute set to: supervisor level, read/write, not present(010 in binary)
	for(i=4; i<1024; i++) {
		pgdir[i] = 0 | PTE_W | PTE_U; 
	};

    // int handler
    set_hwint(0x0E, do_page_fault);

    // write page directory to cr3 and enable PE on cr0
    flush_cr3(pgdir);
    page_enable();
}

