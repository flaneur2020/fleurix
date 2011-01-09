#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <mm.h>

/*
 * the map for page frames. Each physical page is associated with one reference
 * count, and it's free on 0. Only 0 can be allocated via alloc_page().  
 * Reference count is increased on a fork.
 *
 * note: the kernel pages(0~LO_MEM) are initialized as 100 (in page_init()) to 
 * prevent any allocation.
 * */
uchar coremap[NPAGE] = {0, };

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
    panic("no free page.\n");
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
