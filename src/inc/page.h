
struct page {
    uchar          pg_flag;
    uchar          pg_count;   /* ref count */
    ushort         pg_num;
    struct page   *pg_next;
};

extern struct page pgfreelist;
extern struct page coremap[NPAGE];

#define PG_RSVD     0x2 // reserved pages, like kernel memory & bios
#define PG_DMAP     0x4 // TODO: directed mapped to physical addresses

#define PG_ADDR(pg) ((uint)((pg)->pg_num<<12))
