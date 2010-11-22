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

/* root lies on mnt[0].
 * */
struct super* mount_root(short dev){
    struct buf *bp;
    struct super *sp;

    sp = &mnt[0];
    bp = bread(dev, 1);
    memcpy(sp, bp->b_addr, sizeof(struct d_super));
    brelse(bp);
    sp->s_dev = dev;
    if (sp->s_magic!=SB_MAGIC) {
        panic("not an availible dev");
        return NULL;
    }
    return sp;
}

/* */
void do_mount(){
}
