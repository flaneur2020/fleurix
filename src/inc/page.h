
struct page {
    uchar          pg_flag;
    uchar          pg_count;   /* ref count */
    ushort         pg_num;
    struct page   *pg_next;
};

extern struct page pgfreelist;
extern struct page coremap[NPAGE];

extern uint *pgdir;
