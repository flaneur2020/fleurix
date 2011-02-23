#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <page.h>
#include <vm.h>
#include <malloc.h>

/*
 * malloc.c
 * malloc could simplify the implemention of your kernel.
 *
 * However, this routine kmalloc() should returns a *virutal* memory
 * block, whose backend is a page-level allocator -- pgalloc().
 * And the memory block should be aligned with a 2-powered boundary.
 *
 * */

/*
 * the bucket table, each slot got a different 2-powered size. 
 * 1 << 3 == 32,  1 << 12 == 0x1000
 * */
struct bucket bktab[] = {
    32, 0, NULL, NULL,
    64, 0, NULL, NULL,
    128, 0, NULL, NULL,
    256, 0, NULL, NULL,
    512, 0, NULL, NULL,
    1024, 0, NULL, NULL,
    2048, 0, NULL, NULL,
    4096, 0, NULL, NULL
};

struct bucket bkfreelist = {0, };

uint kbrk(){
    struct page *pp;
    pp = pgalloc();
    return pp->pg_num << 12;
}

/* ---------------------------------------------------------- */

inline int bkslot(int size){
    int n,i; 

    if (size <= 0)
        return -1;
    n = 16;
    i = 0;
    while((n <<= 1)<=4096) {
        if (size <= n){
            return i;
        } 
        i++;
    }
    return -1;
}

/* 
 * Allocate one bucket. All buckets are 
 * */
struct bucket *bkalloc(){
    struct bucket *bk, *bh;
    uint page, page_end;

    if (bkfreelist.bk_next == NULL) {
        bh = &bkfreelist;
        page = kbrk();
        page_end = page + PAGE;
        cli();
        for (bk = (struct bucket *)page; (uint)bk < page_end; bk++) {
            bh->bk_next = bk;
            bk->bk_next = NULL;
            bh = bk;
        }
        sti();
    }
    // take the head.
    bk = bkfreelist.bk_next;
    bkfreelist.bk_next = bk->bk_next;
    return bk;
}

int bkfree(struct bucket *bk){
    cli();
    bk->bk_next = bkfreelist.bk_next;
    bkfreelist.bk_next = bk;
    sti();
}

int bkinit(struct bucket *bk, int size){
    uint page;
    int i;
    struct bkentry *beh, *be;

    page = kbrk(); 
    bk->bk_page = page;
    bk->bk_size = size;
    bk->bk_entry = beh = (struct bkentry *)page;
    for (i=0; i<PAGE/size; i++) {
        be = (struct bkentry*)(page + i*size);
        beh->bke_next = be; // #GPF here
        be->bke_next = NULL;
        beh = be;
    }
    return 0;
}

/* -------------------------------------------------------- */

/* 
 * kmalloc().
 * 
 * */
void* kmalloc(uint size){
    struct bucket *bk, *bh;
    struct bkentry *be, *beh;
    int sn, i;
    uint page;

    sn = bkslot(size);
    if (sn < 0) {
        panic("kmalloc(): bad size");
    }
    
    bk = bh = &bktab[sn];
    size = bh->bk_size;
_find:
    // tranverse each bucket
    while((bk = bk->bk_next) != NULL){
        // got free entry
        if (bk->bk_entry != NULL) {
            be = bk->bk_entry;
            bk->bk_entry = be->bke_next;
            return (void*)be;
        }
    }
    // have no free bucket and no free entry
    bk = bkalloc();
    bkinit(bk, size);
    bk->bk_next = bh->bk_next;
    bh->bk_next = bk;
    goto _find;
}

/*
 * 
 * */
int kfree(void* addr, uint size){
    int sn;
    uint page;
    struct bucket *bh, *bk;
    struct bkentry *be;

    sn = bkslot(size);
    if (sn < 0) {
        panic("kfree(): bad size");
    }
    page = PPN(addr) << 12;
    
    bk = bh = &bktab[sn];
    size = bh->bk_size;
    be = (struct bkentry *)addr;
    while((bk = bk->bk_next) != NULL) {
        if (bk->bk_page == page) {
            be->bke_next = bk->bk_entry;
            bk->bk_entry = be;
            return 0;
        }
    }
    panic("kfree(): not avail addr");
}
