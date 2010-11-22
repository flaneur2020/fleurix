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
    struct mount *mp;

    bp = bread(dev, 1);
    mp = &mnt[0];
    mp->m_dev = dev;
    memcpy(&(mp->m_super), bp->b_addr, sizeof(struct d_super));
    brelse(bp);
    if (mp->m_super.s_magic!=SB_MAGIC) {
        panic("not an availible dev");
        return NULL;
    }
    return &(mp->m_super);
}

/* */
void do_mount(){
}
