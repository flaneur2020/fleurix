#ifndef MALLOC_H
#define MALLOC_H

struct bkentry {
    struct bkentry      *bke_next;
};

struct bucket {
    uint                 bk_size;       // each entry's size of this bucket
    void                *bk_page;       // the virtual address of each bucket's page.
    struct bucket       *bk_next;       // the next bucket
    struct bkentry      *bk_entry;      // the first free entry of this bucket, NULL on full.
};

#endif
