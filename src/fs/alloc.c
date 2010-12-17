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

/* alloc one disk block.
 * returns -1 on error.
 * TODO: here assumes each zone equals one block.
 * */
int balloc(ushort dev){
    struct super *sp;
    struct buf *bp;
    int r, bn;

    sp = getsp(dev);
    for(bn=0; bn < sp->s_max_zone; bn++){
        bp = bread(dev, BBLK(sp, bn));
        r = find_bit(bp->b_data, BSIZE);
        if (r < 0) {
            brelse(bp);
            return -1;
        }
        bp->b_data[r/8] |= 1<<(r%8);
        bwrite(bp);
        brelse(bp);
        return BBLK(sp, r);
    }
    unlock_super(sp);
}

void bfree(uint nr){
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

