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

/* See if the block is associated with some buffer. */
struct buf* incore(ushort dev, uint blkno){
    struct devtab *dp;
    struct buf *bp;

    dp = bdevsw[MAJOR(dev)].d_tab;
    for (bp=dp->b_next; bp!=(struct buf*)dp; bp = bp->b_next){
        if (bp->b_dev==dev && bp->b_blkno==blkno) {
            return bp;
        }
    }
    return NULL;
}

/* TODO: ignored the DELWRI scenary. */
struct buf* getblk(int dev, uint blkno){
    struct buf *bp;
    struct devtab *dp;

    if (MAJOR(dev) >= nblkdev) {
        panic("error devno.");
    }

_loop: 
    if (dev < 0){
        dp = (struct devtab*) &bfreelist;
    }
    else {
        dp = bdevsw[MAJOR(dev)].d_tab;
        if (dp==NULL){
            panic("error devtab.");
        }
        // 1. found in the dev's cache list
        if (bp=incore(dev, blkno)) {
            // 2. found in the dev's cache list but busy
            if (bp->b_flag &= B_BUSY) {
                bp->b_flag |= B_WANTED;
                sleep(bp);
                goto _loop;
            } 
            notavail(bp);
            return bp;
        }
    }
    // 3. not found in the dev's cache list, and free list is empty
    if (bfreelist.av_next==&bfreelist) {
        bfreelist.b_flag |= B_WANTED;
        sleep(&bfreelist);
        goto _loop;
    }
    // 4. feel free to take something from the free list.
    notavail(bp = bfreelist.av_next);
    // take it from the dev's cache list and 
    bp->b_prev->b_next = bp->b_next;
    bp->b_next->b_prev = bp->b_prev;
    bp->b_next = dp->b_next;
    bp->b_prev = (struct buf *)dp;
    // insert it into the target dev's cache list's head.
    dp->b_next = bp;
    dp->b_next->b_prev = bp;
    bp->b_dev = dev;
    bp->b_blkno = blkno;
    return bp;
}

/* Release the buffer, with no IO implied.
 * Aka put the buffer back into the freelist.
 * */
void brelse(struct buf *bp){
    struct buf *tmp;

    if (bp->b_flag & B_WANTED) {
        wakeup(bp);
    }
    if (bfreelist.b_flag & B_WANTED) {
        bfreelist.b_flag &= ~B_WANTED;
        wakeup(&bfreelist);
    }
    bp->b_flag &= ~(B_WANTED|B_BUSY|B_ASYNC);
    bp->av_next = &bfreelist;
    bp->av_prev = bfreelist.av_prev;
    bp->av_prev->av_next = bp;
}

/**********************************************/

/* Unlink a buffer from the free list and mark it busy.*/
void notavail(struct buf *bp){
    bp->av_prev->av_next = bp->av_prev;
    bp->av_next->av_prev = bp->av_next;
    bp->b_flag |= B_BUSY;
}

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
 *
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
        bp->b_dev = -1;
        bp->b_addr = buffers[i];
        bp->b_flag = B_BUSY;
        brelse(bp);
    }

    nblkdev = 0;
    for(bsp=&bdevsw[0]; bsp->d_open!=0; bsp++){
        dtp = bsp->d_tab;
        dtp->b_next  = dtp->b_prev  = (struct buf *) dtp;
        dtp->av_next = dtp->av_prev = (struct buf *) dtp;
        nblkdev++;
    }
}

