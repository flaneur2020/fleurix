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
#include <stat.h>

/*
 * this file indicated the read/write operations of the inodes.
 * TODO: ignored special files yet.
 * */

/* 
 * read data from a locked inode.
 * returns -1 on error.
 * */
int readi(struct inode *ip, char *buf, uint off, uint cnt){
    struct buf *bp;
    uint tot=0, m=0, bn=0;

    // file size limit 
    if ((off > ip->i_size) || (off+cnt < off)){
        cu->p_error = E2BIG;
        return -1;
    }
    if (off+cnt > ip->i_size) {
        cnt = ip->i_size - off;
    }
    // read
    for(tot=0; tot<cnt; tot+=m, off+=m, buf+=m){
        m = min(cnt - tot, BLK - off%BLK);
        bn = bmap(ip, off/BLK, 0);
        if (bn == 0) {
            memset(bp->b_data + off%BLK, 0, m);
        }
        else {
            bp = bread(ip->i_dev, bn);
            memcpy(buf, bp->b_data + off%BLK, m);
            brelse(bp);
        }
    }
    return cnt;
}

/*
 * write data to a regular file.
 * */
int writei(struct inode *ip, char *buf, uint off, uint cnt){
    struct buf *bp;
    struct super *sp;
    uint tot=0, m=0, bn=0;

    if (off+cnt < off){
        return -1;
    }
    if (off+cnt > MAX_FILESIZ) {
        cnt = MAX_FILESIZ - off;
    }
    // do write.
    for(tot=0; tot<cnt; tot+=m, off+=m, buf+=m){
        m = min(cnt - tot, BLK - off%BLK);
        bn = bmap(ip, off/BLK, 1);
        if (bn==0) {
            panic("bad block.");
        }
        else {
            bp = bread(ip->i_dev, bn); // note here!
            memcpy(bp->b_data + off%BLK, buf, m);
            bwrite(bp);
            brelse(bp);
        }
    }
    // adjust the inode's file size
    if (cnt > 0 && off > ip->i_size) {
        ip->i_size = off;
        iupdate(ip);
    }
    return cnt;
}
