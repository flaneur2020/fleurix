#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>
#include <buf.h>
#include <conf.h>

/* buf bodies. Make it start at 0x100000 alright!  */
extern char buffers[NBUF][512];

/* buf heads */
struct buf   buf[NBUF];

/* */
struct buf  bfreelist;

/* Initialized in buf_init. */
int nblkdev = 0;

/**********************************************/

/* TODO: here. */
struct buf* getblk(ushort devno, uint blkno){
}

/* buffer release */
void brelse(struct buf *bp){
    if (bp->b_flag & B_WANTED) {
        wakeup(bp);
    }
    if (bfreelist.b_flag & B_WANTED) {
        bfreelist.b_flag &= ~B_WANTED;
    }
}

/* */
int incore(ushort devno, uint blkno){
}

/**********************************************/

void iowait(struct buf *bp){
    sleep(bp);
}

void iodone(struct buf *bp){
    wakeup(bp);
}

/**********************************************/

/*
 * Read in (if necessary) the block and return a buffer pointer
 * */
struct buf* bread(int dev, uint blkno){
    struct buf *bp;
    bp = getblk(dev, blkno);
    if (bp->b_flag & B_DONE) {
        return bp;
    }
    bp->b_flag != B_READ;
    (*bdevsw[MAJOR(dev)].d_request)(bp);
    iowait(bp);
    return bp;
}

void bwrite(struct buf *bp) {
}

/**********************************************/

/*
 * initialize the buffer IO system by freeing all buffers
 * and setting all device buffer lists to empty.
 * */
void buf_init() {
    uint i=0;
    struct buf *bp;
    struct bdevsw *bsp;
    struct devtab *dtp;

    bfreelist.b_prev = bfreelist.b_next = &bfreelist;
    bfreelist.av_prev = bfreelist.av_next = &bfreelist;
    for(i=0; i<NBUF; i++){
        bp = &buf[i];
        bp->b_dev = 0;
        bp->b_addr = buffers[i];
        bp->b_next = bfreelist.b_next;
        bfreelist.b_next->b_prev = bp;
        bp->b_flag = B_BUSY;
        brelse(bp);
    }

    panic("");
    return;
    nblkdev = 0;
    for(bsp=&bdevsw[0]; bsp->d_open!=0; bsp++){
        dtp = bsp->d_tab;
        dtp->b_head = (struct buf *) dtp;
        dtp->b_tail = (struct buf *) dtp;
        dtp->av_head = (struct buf *) dtp;
        dtp->av_tail = (struct buf *) dtp;
        nblkdev++;
    }
    printf("buf_init(): nblkdev=\n", nblkdev);
}

