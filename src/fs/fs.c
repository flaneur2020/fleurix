#include <param.h>
#include <x86.h>
#include <proto.h>
//
#include <buf.h>
#include <conf.h>
//
#include <super.h>
#include <inode.h>
#include <fs.h>

struct mount mnt[NMOUNT];

/* root lies on mnt[0].
 * */
struct super* mount_root(short dev){
    struct buf *bp;

    bp = bread(dev, 1);
    mnt[0].m_dev = dev;
    mnt[0].m_super = *(struct super*)bp->b_addr;
    brelse(bp);
    return &(mnt.m_super);
}

/* */
void do_mount(){
}
