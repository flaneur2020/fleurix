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

/**********************************************/

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
}

void iodone(struct buf *bp){
}

/**********************************************/

/*
 * initialize the buffer IO system by freeing all buffers
 * and setting all device buffer lists to empty.
 * */
void buf_init() {
    uint i;
    struct buf *bp;

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
    
}

