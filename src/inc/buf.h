/* */

struct buf {
    uint            b_flag;
    struct buf     *b_next;
    struct buf     *b_prev;
    struct buf     *av_next;
    struct buf     *av_prev;
    short           b_dev;
    uint            b_blkno;
    char           *b_data;
    int             b_error;
};

extern struct buf   buff[NBUF];

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
 *
 * note: Little trick here, devtab's header can be also 
 * visited via a struct buf *.
 * */
struct devtab {
    uint            d_active;             /* busy flags */
    struct buf      *b_next;            /* first buf for this dev */
    struct buf      *b_prev;            /* last  buf for this dev */
    struct buf      *av_next;           /* head of IO queue */
    struct buf      *av_prev;           /* tail of IO queue */
    uint            d_errcnt;           /* error count (for recovery) */
};

/*
 * This is the head of the freelist.
 * */
extern struct buf   bfreelist;

#define B_WRITE     0x0
#define B_READ      0x1
#define B_DONE      0x2
#define B_ERROR     0x4
#define B_BUSY      0x8
#define B_WANTED    0x10
#define B_DIRTY     0x20


