#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <buf.h>
#include <conf.h>

/* buf bodies. Make it start at 0x100000 alright!  */
extern char buffers[NBUF][BLK];

/* buf heads */
struct buf   buf[NBUF];

/* */
struct buf  bfreelist;

/* Initialized in buf_init. */
int nblkdev = 0;

/**********************************************/

/* See if the block is associated with some buffer. */
struct buf* incore(ushort dev, uint blkno){
    struct devtab *dtp;
    struct buf *bp;

    dtp = bdevsw[MAJOR(dev)].d_tab;
    for (bp=dtp->b_next; bp!=(struct buf*)dtp; bp=bp->b_next){
        if (bp->b_dev==dev && bp->b_blkno==blkno) {
            return bp;
        }
    }
    return NULL;
}

/* allocate a block.
 *
 * noet: getblk *ALWAYS* returns a B_BUSY block, so the other processes
 * can not access this. so never forget brelse it after a getblk, just 
 * as malloc/free.
 * so codes like
 *      bp = bread(dev, 1);
 *      bp = bread(dev, 1); 
 * may sleep forever.
 *
 * TODO: ignored the DELWRI scenary.
 * */
struct buf* getblk(int dev, uint blkno){
    struct buf *bp;
    struct devtab *dtp;

    if (MAJOR(dev) >= nblkdev) {
        panic("error devno.");
    }

_loop: 
    if (dev < 0){
        dtp = (struct devtab*) &bfreelist;
    }
    else {
        dtp = bdevsw[MAJOR(dev)].d_tab;
        if (dtp==NULL){
            panic("error devtab.");
        }
        // 1. found in the dev's cache list
        if (bp=incore(dev, blkno)) {
            // 2. found in the dev's cache list but busy
            if (bp->b_flag & B_BUSY) {
                bp->b_flag |= B_WANTED;
                sleep(bp, PRIBIO);
                goto _loop;
            } 
            notavail(bp);
            return bp;
        }
    }
    // 3. not found in the dev's cache list, and free list is empty
    if (bfreelist.av_next==&bfreelist) {
        bfreelist.b_flag |= B_WANTED;
        sleep(&bfreelist, PRIBIO);
        goto _loop;
    }
    // 4. feel free to take something from the free list (head).
    bp = bfreelist.av_next;
    notavail(bp);
    bp->b_flag = B_BUSY;
    // take it from the old dev's cache list and 
    bp->b_prev->b_next = bp->b_next;
    bp->b_next->b_prev = bp->b_prev;
    bp->b_next = dtp->b_next;
    bp->b_prev = (struct buf *)dtp;
    // prepend it into the target dev's cache list.
    bp->b_prev = dtp;
    bp->b_next = dtp->b_next;
    dtp->b_next->b_prev = bp;
    dtp->b_next = bp;
    // 
    bp->b_dev = dev;
    bp->b_blkno = blkno;
    return bp;
}

/* Release the buffer, with no IO implied.
 * Aka put the buffer back (append) into the freelist.
 * note: brelse is not a user-transparency routine, remember
 * calling this after a getblk.
 * TODO: consider B_ERROR
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
    if (bp->b_flag & B_ERROR) {
        bp->b_dev = NODEV;
    }
    bp->b_flag &= ~(B_WANTED|B_BUSY);
    bp->av_next = &bfreelist;
    bp->av_prev = bfreelist.av_prev;
    bp->av_prev->av_next = bp;
    bp->av_next->av_prev = bp;
}

/**********************************************/

/* unlink a buffer from the free list and mark it busy.*/
void notavail(struct buf *bp){
    bp->av_prev->av_next = bp->av_next;
    bp->av_next->av_prev = bp->av_prev;
    bp->b_flag |= B_BUSY;
}

void iowait(struct buf *bp){
    while((bp->b_flag&B_DONE)==0){
        sleep(bp, PRIBIO);
    }
}

void iodone(struct buf *bp){
    bp->b_flag |= B_DONE;
    bp->b_flag &= ~B_WANTED; 
    //brelse(bp);
    wakeup(bp);
}

/**********************************************/

/*
 * Read in (if necessary) the block and return a buffer pointer.
 * note: we assume all the bread() calls in kernel is correct, but the
 * calls from a syscall is suspected.
 * */
struct buf* bread(int dev, uint blkno){
    struct buf *bp;
    bp = getblk(dev, blkno);
    if (bp->b_flag & B_DONE) { 
        return bp;
    }
    bp->b_flag |= B_READ;
    (*bdevsw[MAJOR(dev)].d_request)(bp);
    iowait(bp);
    return bp;
}

/* Write the buffer, waiting for completion.
 * Then release the buffer.
 * TODO: write though right now. make it write back later.
 */
void bwrite(struct buf *bp) {
	bp->b_flag &= ~(B_READ | B_DONE | B_ERROR | B_DIRTY);
    (*bdevsw[MAJOR(bp->b_dev)].d_request)(bp);
    iowait(bp);
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
        bp->b_dev = NODEV;
        bp->b_data = buffers[i];
        bp->b_flag = B_BUSY;
        bp->b_next = bp->b_prev = bp;
        brelse(bp);
    }

    nblkdev = 0;
    for(bsp=&bdevsw[0]; bsp<&bdevsw[NBLKDEV]; bsp++){
        dtp = bsp->d_tab;
        dtp->b_next  = dtp->b_prev  = (struct buf *) dtp;
        dtp->av_next = dtp->av_prev = (struct buf *) dtp;
        nblkdev++;
    }
}

/**************************************************/

void dump_buf(struct buf *bp){
    int i, c;
    for(i=0; i<512; i++){
        c = bp->b_data[i];
        if (c>31 && c<127){
            printf("%c", c);
        }
        else {
            printf("%x", c);
        }
    }
}
