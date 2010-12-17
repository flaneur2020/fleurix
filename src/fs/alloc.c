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

/* alloc one disk block. returns its LBA
 * TODO: here assumes each zone equals one block.
 * */
int balloc(ushort dev){
    struct super *sp;
    struct buf *bp;
    int bn, r, i;

    sp = getsp(dev);
    for(i=0; i < sp->s_max_zone; i++){
        bp = bread(dev, BBLK(sp, i));
        r = find_bit(bp->b_data, BSIZE);
        if (r < 0) {
            brelse(bp);
            continue;
        }
        bn = i*BPB + r;
        bp->b_data[bn/8] |= 1<<(bn%8);
        bwrite(bp);
        brelse(bp);
        unlk_sp(sp);
        return r;
    }
    unlk_sp(sp);
    panic("no free block");
    return -1;
}

int bfree(ushort dev, uint bn){
    struct buf *bp;
    struct super *sp;
    
    sp = getsp(dev);
    bp = bread(dev, BBLK(sp, bn));
    if (bp->b_data[bn/8]==0){
        panic("freeing free block");
    }
    bp->b_data[bn/8] &= ~(1<<(bn&8));
    bwrite(bp);
    brelse(bp);
    unlk_sp(sp);
}

/* zero one disk block */
int bzero(uint bn){
}

/* on inodes */
uint ialloc(){
}

void ifree(){
}

/* find the first zero bit of one bitmap */
int find_bit(char *bm, int size){
    uint byte, i, j;
    for (i=0; i<size; i++){
        byte = bm[i];
        //if (0 == ~byte)
            //continue;
        for (j=0; j<8; j++){
            if (byte & (1 << j)){
                return i*8 + j;
            }
        }
    }
    return -1;
}

