#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <page.h>

/*
 * the map for page frames. Each physical page is associated with one reference
 * count, and it's free on 0. Only 0 can be allocated via pgalloc().  
 * Reference count is increased on a fork.
 *
 * */
struct page coremap[NPAGE];
struct page pgfreelist;

/* returns the page struct via a physical page number. */
struct page* pgfind(uint pn){
    int nr;
    nr = pn-LO_MEM/PAGE;
    if (nr<0 || nr>=NPAGE ) {
        panic("bad page number.");
    }
    return &coremap[nr];
}

/* 
 * allocate an free physical page. 
 * get the head of the freelist, remove it and increase the refcount.
 * */ 
struct page* pgalloc(){
    struct page *pp;

    if (pgfreelist.pg_next == NULL) {
        panic("no free page.\n");
        return NULL;
    }
    
    cli();
    pp = pgfreelist.pg_next;
    pp->pg_count = 1;
    pgfreelist.pg_next = pp->pg_next;
    sti();
    return pp;
}

/*
 * free a physical page. decrease the reference count and put it back
 * to the freelist. 
 */
int pgfree(struct page *pp){
    if (pp->pg_count==0) {
        panic("freeing a free page.");
    }

    cli();
    pp->pg_count--;
    if (pp->pg_count==0) {
        pp->pg_next = pgfreelist.pg_next;
        pgfreelist.pg_next = pp;
    }
    sti();
    return pp->pg_num;
}

/*
 * map a linear address to physical address.  
 * TODO: pgattach().
 * */
int pgattach(struct pte *pgdir, struct page *pp, uint vaddr, uint flag){
}

/* initialize pages' free list. */
int pm_init(){
    struct page *pp, *ph;
    uint i, pn;

    pn = LO_MEM/PAGE + 1;
    ph = &pgfreelist;
    for (pp=&coremap[0]; pp<&coremap[NPAGE]; pp++) {
        pp->pg_flag = 0;
        pp->pg_count = 0;
        pp->pg_num = pn++;
        //
        pp->pg_next = NULL;
        ph->pg_next = pp;
        ph = pp;
    }
}
