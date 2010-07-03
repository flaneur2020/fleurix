
#include <param.h>
#include <x86.h>
#include <sys.h>

// the ONLY page directory
// which indicates the linear address space of 4GB
// each proc have a address space of 64MB, which starts at pid*64mb
uint *pdir = (uint *) 0x9C000;

uchar frmmap[NFRAME] = {0, };

/**************************************************************/

void do_page_fault(struct regs *r){
}

void do_no_page(struct regs *r){
}

void do_wp_page(struct regs *r){
}

/**************************************************************/

void put_page(uint la, uint pa){
    uint *ptab = pdir[la>>12];
    if (! ptab & 1){
        ptab = palloc();
        ptab = ((uint)ptab) |  3;
        pdir[PDX(la)] = ptab;
    }
    ptab[PTX(la)] = pa | 3;
    frmmap[pa/4096]++;
}

void copy_page(uint la1, uint la2){
}

void copy_ptab(){
}

/**************************************************************/
// Allocate/Free a Frame


// tranverse frmmap, if 0, set it 1 and return the address
// if no frame availible, return 0
uint palloc(){
    int i;
    for(i=0; i<NFRAME; i++){
        if(frmmap[i]==0){
            frmmap[i] = 1;
            return LO_MEM + i*4096;
        }
    }
    return 0;
}

// decrease the target in frmmap. 
uint pfree(uint addr){
    int i;
    i = (addr-LO_MEM) / 4096;
    if(frmmap[i]==0){
        return 0;
    }
    frmmap[i]--;
    return i;
}

/***************************************************************/

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
    // address of a page directory have to make aligned to multiple of 4kb
    // the page table comes right after the page directory, 0x1000 == 1024 * 4
	uint *ptab = (uint *) 0x9D000; 
	uint addr  = 0; 

	// map the top 16MB of memory
    //
    // fill the top four entry of the page directory
    // attribute set to: supervisor level, read/write, present(011 in binary)
	uint i, j;
    for(i=0; i<4; i++){
        ptab = 0x9D000 + 0x1000*i;
        pdir[i] = (uint)ptab | 3;
        for(j=0; j<1024; j++) {
            ptab[j] = addr | 3; 
            addr += 4096; // 4096 = 4kb
        }
    }

	// fill the rest of the page directory
    // attribute set to: supervisor level, read/write, not present(010 in binary)
	for(i=4; i<1024; i++) {
		pdir[i] = 0 | 2; 
	};

    // write page directory to cr3 and enable PE on cr0
    flush_cr3(pdir);
    page_enable();
}

