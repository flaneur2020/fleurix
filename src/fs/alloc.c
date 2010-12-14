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

    sp = get_super(dev);
}

void bfree(uint nr){
}

/* on inodes */
uint ialloc(){
}

void ifree(){
}

/* find the first zero bit of one bitmap */
int bm_find(char *bm, int size){
}

