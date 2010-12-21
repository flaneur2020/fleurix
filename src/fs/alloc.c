#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
// 
#include <buf.h>
#include <conf.h>
//
#include <super.h>
#include <inode.h>

/* alloc one disk block in order to extend one file. returns 
 * its LBA.
 * note: only called in bmap(, , 1). 
 * note2: here assumes each zone equals one block.
 * */
int balloc(ushort dev){
    struct super *sp;
    struct buf *bp;
    int nr, r, bn;

    sp = getsp(dev);
    for(nr=0; nr < sp->s_max_zone; nr++){
        bp = bread(dev, BMAPBLK(sp, nr));
        r = find_bit(bp->b_data, BSIZE);
        if (r < 0) {
            continue;
        }
        bn = nr*BPB + r;
        bp->b_data[bn/8] |= 1<<(bn%8);
        bwrite(bp);
        brelse(bp);
        unlk_sp(sp);
        bzero(dev, sp->s_data_zone0 + bn);
        return sp->s_data_zone0 + bn;
    }
    unlk_sp(sp);
    panic("no free block");
    return -1;
}

/* free a block. 
 * */
int bfree(ushort dev, uint nr){
    struct buf *bp;
    struct super *sp;
    uint bn;
    
    sp = getsp(dev);
    if ((nr < sp->s_data_zone0) || (nr >= sp->s_max_zone)) {
        panic("freeing a block not in data zone");
    }
    bn = nr - sp->s_data_zone0;
    bp = bread(dev, BMAPBLK(sp, bn));
    if ((bp->b_data[bn/8] & (1<<(bn%8))) == 0){
        panic("freeing free block");
    }
    bp->b_data[bn/8] &= ~(1<<(bn%8));
    bwrite(bp);
    brelse(bp);
    unlk_sp(sp);
}

/* zero one disk block. only called in balloc().
 * */
int bzero(ushort dev, uint bn){
    struct buf *bp;

    bp = getblk(dev, bn);
    memset(bp->b_data, 0, BSIZE);
    bwrite(bp);
    brelse(bp);
    return 0;
}

/* --------------------------------------------------- */

/* allocate one inode 
 * this may easier to debug.
 * */
uint ialloc(ushort dev){
}

void ifree(ushort dev, uint ino){
}

/* find the first zero bit in one bitmap */
int find_bit(char *bm, int size){
    uint byte, i, j;
    for (i=0; i<size; i++){
        byte = bm[i];
        if (0 == ~byte)
            continue;
        for (j=0; j<8; j++){
            if ((byte & (1 << j))==0){
                return i*8 + j;
            }
        }
    }
    return -1;
}

