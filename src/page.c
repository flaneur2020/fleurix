#include <sys.h>
#include <x86.h>

uchar frmtab[16*1024];

void flush_cr3(uint addr){
    asm volatile("mov %0, %%cr3":: "r"(addr));
}

void page_enable(){
    uint cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void page_init(){
    // a page directory have to make aligned to multiple of 4kb
    // the page table comes right after the page directory
	uint *pdir = (uint *) 0x9C000;
	uint *ptab = (uint *) 0x9D000; 
	uint addr  = 0; 

	// map the first 4MB of memory
	uint i;
	for(i=0; i<1024; i++) {
		ptab[i] = addr | 3; 
		addr = addr + 4096; // 4096 = 4kb
	};

	// fill the first entry of the page directory
    // attribute set to: supervisor level, read/write, present(011 in binary)
	pdir[0] = (uint)ptab | 3; 
	// fill the rest of the page directory
    // attribute set to: supervisor level, read/write, not present(010 in binary)
	for(i=1; i<1024; i++) {
		pdir[i] = 0 | 2; 
	};

    // write page directory to cr3 and enable PE on cr0
    flush_cr3(pdir);
    page_enable();
}

