
/* */

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

/*
 * Each block device has a devtab, which contains private
 * state stuff and 2 list head: the b_head/b_tail list, 
 * which is doubly linked and has all the buffers currently 
 * associated with that major device;
 * and the av_head/av_tail list, which is private to the 
 * device but in fact is always used for the head and tail
 * of the IO queue for the device.
 * Various routines in bio.c look at b_head/b_tail
 * but the rest is private to each device driver. 
 * */
struct devtab {
    char            d_active;           /* busy flags */
    char            d_errcnt;           /* error count (for recovery) */
    struct buf      *b_head;            /* first buf for this dev */
    struct buf      *b_tail;            /* last  buf for this dev */
    struct buf      *av_head;           /* head of IO queue */
    struct buf      *av_tail;           /* tail of IO queue */
}

/*
 * This is the head of the freelist.
 * */
extern struct buf   bfreelist;

#define B_WRITE     0x0
#define B_READ      0x1
#define B_DONE      0x2
#define B_ERROR     0x4
#define B_BUSY      0x8
#define B_PHYS      0x10

#define B_MAP       0x20

#define B_WANTED    0x40

#define B_RELOC     0x80
#define B_ASYNC     0x100

#define B_DELWRI    0x200


