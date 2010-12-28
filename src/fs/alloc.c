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
        // bzero(dev, sp->s_data_zone0 + bn);
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

/* zero one disk block.
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

/* allocate one inode. returns the inode number which can be
 * fetched with via an iget.
 * */
int ialloc(ushort dev){
    uint nr, ino;
    int r;
    struct inode *ip;
    struct super *sp;
    struct buf *bp;

    sp = getsp(dev);
    for (nr=0; nr < sp->s_max_inode; nr++){
        bp = bread(dev, IMAPBLK(sp, nr));
        r = find_bit(bp->b_data, BSIZE);   
        if (r < 0) {
            continue;
        }
        ino = nr*BPB + r;
        bp->b_data[ino/8] |= 1<<(ino%8);
        bwrite(bp);
        brelse(bp);
        unlk_sp(sp);
        return ino;
    }
    unlk_sp(sp);
    panic("no free inode");
    return -1;
}

void ifree(ushort dev, uint ino){
    struct buf *bp;
    struct super *sp;
    
    sp = getsp(dev);
    if ((ino <= 0) || (ino >= sp->s_max_inode)) {
        panic("freeing an non-existing inode");
    }
    bp = bread(dev, IMAPBLK(sp, ino));
    if ((bp->b_data[ino/8] & (1<<(ino%8))) == 0){
        panic("freeing free inode");
    }
    bp->b_data[ino/8] &= ~(1<<(ino%8));
    bwrite(bp);
    brelse(bp);
    unlk_sp(sp);
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

