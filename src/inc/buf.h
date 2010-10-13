
/* */
struct devno {
    uchar           minor;
    uchar           major;
}

struct buf {
    uint            b_flag;
    struct buf      *b_next;
    struct buf      *b_prev;
    struct buf      *av_next;
    struct buf      *av_prev;
    struct devno    b_dev;
    uint            b_blkno;
    uint            b_addr;
    uint            b_xmem;
    uint            b_wcount;
    char            b_error;            
    uint            b_resid;        
}

extern struct buf   buf[NBUF];

extern struct buf   *bfreelist;
