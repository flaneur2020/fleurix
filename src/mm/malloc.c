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
    { 32, 0, NULL, NULL },
    { 64, 0, NULL, NULL },
    { 128, 0, NULL, NULL },
    { 256, 0, NULL, NULL },
    { 512, 0, NULL, NULL },
    { 1024, 0, NULL, NULL },
    { 2048, 0, NULL, NULL },
    { 4096, 0, NULL, NULL }
};

struct bucket bkfreelist = {0, };

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
 * Allocate one bucket. All free buckets are cached in one free list,
 * if no free bucket, allocate one page and divide it into buckets.
 * */
struct bucket *bkalloc(){
    struct bucket *bk, *bh;
    struct page *pg;
    uint page, page_end;

    // no free bucket, allocate one page and divide it into buckets
    if (bkfreelist.bk_next == NULL) {
        bh = &bkfreelist;
        pg = pgalloc();
        page = pg->pg_num*PAGE;
        page_end = page + PAGE;
        cli();
        for (bk = (struct bucket *)page; (uint)bk < page_end; bk++) {
            bh->bk_next = bk;
            bk->bk_next = NULL;
            bh = bk;
        }
        sti();
    }
    // got free bucket, take the head.
    bk = bkfreelist.bk_next;
    bkfreelist.bk_next = bk->bk_next;
    return bk;
}

/* free one bucket back to the free list. */
int bkfree(struct bucket *bk){
    cli();
    bk->bk_next = bkfreelist.bk_next;
    bkfreelist.bk_next = bk;
    sti();
    return 0;
}

int bkinit(struct bucket *bk, int size){
    uint page;
    int i;
    struct page *pg;
    struct bkentry *beh, *be;

    pg = pgalloc();
    page = (pg->pg_num)*PAGE;
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
 * one page size is special considered, for no need to arrange
 * any bucket. which symmeties in kfree.
 * */
void* kmalloc(uint size){
    struct bucket *bk, *bh;
    struct bkentry *be;
    struct page *pg;
    int sn;

    sn = bkslot(size);
    if (sn < 0) {
        panic("kmalloc(): bad size");
    }

    bk = bh = &bktab[sn];
    size = bh->bk_size;
    // special case for one page size
    if (size==PAGE) {
        pg = pgalloc();
        return (void*)(pg->pg_num * PAGE);
    }
    // normal case:
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
    struct page *pg;
    struct bucket *bh, *bk;
    struct bkentry *be;

    sn = bkslot(size);
    if (sn < 0) {
        panic("kfree(): bad size");
    }
    page = PPN(addr) * PAGE;
    //
    bk = bh = &bktab[sn];
    size = bh->bk_size;
    be = (struct bkentry *)addr;
    // special case for one page size
    if (size==PAGE) {
        pg = pgfind(PPN(addr));
        pgfree(pg);
        return 0;
    }
    // normal case
    while((bk = bk->bk_next) != NULL) {
        if (bk->bk_page == page) {
            be->bke_next = bk->bk_entry;
            bk->bk_entry = be;
            return 0;
        }
    }
    panic("kfree(): not avail addr");
}
