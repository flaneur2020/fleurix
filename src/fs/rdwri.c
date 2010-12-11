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

/*
 * this file indicated the read/write operations of the inodes.
 * */

/* 
 * read data from inode.
 * TODO: special files.
 * */
int readi(struct inode *ip, char *dst, uint off, uint n){
    struct buf *bp;
    uint tot=0, m=0;

    // file size limit 
    if ((off > ip->i_size) || (off+n < off)){
        current->p_error = E2BIG;
        return -1;
    }
    if (off+n > ip->i_size) {
        n = ip->i_size - off;
    }
    // read
    for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
        bp = bread(ip->i_dev, bmap(ip, off/BLK));
        m = min(n - tot, BLK - off%BLK);
        memcpy(dst, bp->b_data + off%BLK, m);
        brelse(bp);
    }
    return n;
}

int writei(){
}
