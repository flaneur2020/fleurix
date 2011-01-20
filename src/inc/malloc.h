#ifndef MALLOC_H
#define MALLOC_H

struct bkentry {
    struct bkentry      *bke_next;
};

/* each bucket is associated with one page-size memory. */
struct bucket {
    uint                 bk_size;       // each entry's size of this bucket
    uint                 bk_page;       // the virtual address of each bucket's page.
    struct bucket       *bk_next;       // the next bucket (each bucket page memory)
    struct bkentry      *bk_entry;      // the first free entry of this bucket, NULL on full.
};

#endif
