#include <param.h>
#include <x86.h>
#include <proc.h>
// 
#include <buf.h>
#include <conf.h>
//
#include <super.h>
#include <inode.h>
#include <stat.h>
//
#include <proto.h>

/*
 * Given an inode and a position within the corresponding file, locate the
 * block (not zone) number in which that position is to be found and return it.
 * returns 0 on error.
 * note: 
 * the first 7 entry of ip->zones[] are direct pointers, ip->zone[7] is an indirect 
 * pointer to a zone map, while ip->zone[8] is an double indirect pointer to a zone map.
 * note2: file extends only here.
 *
 * ip->i_size is adjusted in writei();
 *
 * TODO: maybe we can remove all the updatei and bwrite stuff, but sync() required first.
 */
int bmap(struct inode *ip, ushort nr, uchar creat) {
    struct buf *bp, *bp2;
    short *zmap, *zmap2;
    ushort dev;

    if ((nr > MAX_FILESIZ) || ((nr > (ip->i_size/BLK) && (creat==0)))) {
        panic("blk nr too big.");
    }

    dev = ip->i_dev;
    if (nr<7){
        // if the create flag is set
        if (ip->i_zone[nr]==0 && creat){
            ip->i_zone[nr] = balloc(dev);
            ip->i_flag |= I_DIRTY;
            iupdate(ip);
        }
        return ip->i_zone[nr];
    }
    nr -= 7;
    /*----------------------------*/
    // read the indirect zone map
    if (nr<NINDBLK){
        if (ip->i_zone[7]==0) {
            if (creat==0) {
                return 0;
            }
            // if the indirect block is null and creat is set
            ip->i_zone[7] = balloc(dev);
            bzero(dev, ip->i_zone[7]);
            ip->i_flag |= I_DIRTY;
            iupdate(ip);
        }
        bp = bread(dev, ip->i_zone[7]);
        zmap = (short *)bp->b_data;
        if (zmap[nr]==0 && creat) {
            zmap[nr] = balloc(dev);
            bwrite(bp);
        }
        brelse(bp);
        return zmap[nr];
    }
    /*----------------------------*/
    nr -= NINDBLK;
    // the double indirect zone map.
    // read the middle indirect zone map.
    if (ip->i_zone[8]==0) {
        // if the first indirect block is null and creat is set
        if (creat == 0) {
            return 0;
        }
        ip->i_zone[8] = balloc(dev);
        bzero(dev, ip->i_zone[8]);
        ip->i_flag |= I_DIRTY;
        iupdate(ip);
    } 
    bp = bread(dev, ip->i_zone[8]);
    zmap = (short *)bp->b_data;
    if (zmap[nr/NINDBLK]==0) {
        if (creat==0) {
            brelse(bp);
            return 0;
        }
        // if the second indirect block is null and creat is set
        zmap[nr/NINDBLK] = balloc(dev);
        bzero(dev, zmap[nr/NINDBLK]);
        bwrite(bp);
    }
    // read the secondary indirect zone map.
    bp2 = bread(dev, zmap[nr/NINDBLK]);
    zmap2 = (short*)bp2->b_data;
    if (zmap2[nr%NINDBLK]==0 & creat) {
        zmap2[nr%NINDBLK] = balloc(dev);
        bwrite(bp2);
    }
    brelse(bp);
    brelse(bp2);
    return zmap2[nr%NINDBLK];
}

/*
 * Discard inode's content.
 * Called from routines like open(), iput(). 
 * */
int itrunc(struct inode *ip){
    int i,j; 
    ushort dev;
    struct buf *bp, *bp2;
    char *zmap, *zmap2;

    // only regular file but not directory can be truncated
    if (!(ip->i_mode & S_IFREG) || (ip->i_mode & S_IFDIR)) {
        return -1;
    }
    
    dev = ip->i_dev;
    for (i=0; i<7; i++){
        if (ip->i_zone[i]!=0){
            bfree(dev, ip->i_zone[i]);
            ip->i_flag |= I_DIRTY;
            ip->i_zone[i] = 0;
        }
    }
    /* -------------------------- */
    if (ip->i_zone[7] != 0){
        bp = bread(dev, ip->i_zone[7]);
        zmap = bp->b_data;
        for (i=0; i<NINDBLK; i++){
            if(zmap[i] != 0){
                bfree(dev, zmap[i]);
            }
        }
        bfree(dev, ip->i_zone[7]);
        ip->i_zone[7] = 0;
        brelse(bp);
    }
    /* -------------------------- */
    if (ip->i_zone[8] != 0){
        bp = bread(dev, ip->i_zone[8]);
        zmap = bp->b_data;
        for (i=0; i<NINDBLK; i++) {
            if (zmap[i] != 0){
                bp2 = bread(dev, zmap[i]);
                zmap2 = bp2->b_data;
                for (j=0; j<NINDBLK; j++) {
                    if (zmap2[j] != 0){
                        bfree(dev, zmap2[j]);
                    }
                }
                brelse(bp2);
                bfree(dev, zmap[i]);
            }
        }
        brelse(bp);
        bfree(dev, ip->i_zone[8]);
        ip->i_zone[8] = 0;
    }
    ip->i_size = 0;
    iupdate(ip);
}
