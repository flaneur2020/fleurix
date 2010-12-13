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
 * returns -1 on error.
 *
 * TODO: special files.
 * */
int readi(struct inode *ip, char *buf, uint off, uint cnt){
    struct buf *bp;
    uint tot=0, m=0;

    // file size limit 
    if ((off > ip->i_size) || (off+cnt < off)){
        current->p_error = E2BIG;
        return -1;
    }
    if (off+cnt > ip->i_size) {
        cnt = ip->i_size - off;
    }
    // read
    for(tot=0; tot<cnt; tot+=m, off+=m, buf+=m){
        bp = bread(ip->i_dev, bmap(ip, off/BLK));
        m = min(cnt - tot, BLK - off%BLK);
        memcpy(buf, bp->b_data + off%BLK, m);
        brelse(bp);
    }
    return cnt;
}

int writei(struct inode *ip, char *buf, uint off, uint cnt){
}
