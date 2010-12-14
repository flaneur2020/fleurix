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
#include <fs.h>

/* alloc one disk block */
uint balloc(ushort dev){
    struct super *sp;
    struct buf *bp;

    sp = getsp(dev);
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
        for (j=0; j<8; j++){
            if (byte & (1 << j)){
                return i*8 + j;
            }
        }
    }
    return -1;
}

