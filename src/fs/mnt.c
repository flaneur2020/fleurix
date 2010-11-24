#include <param.h>
#include <x86.h>
#include <proto.h>
//
#include <buf.h>
#include <conf.h>
//
#include <super.h>
#include <inode.h>

/* load a super block in-core. and associate with an in-core inode.
 *
 * if did not got any free slot, just simply raise an error instead
 * of sleep until somebody frees like what getblk does.
 * */
int do_mount(ushort dev, struct inode *ip){
    struct buf *bp;
    struct super *sp;

    for(sp=&mnt[0]; sp<&mnt[NMOUNT]; sp++){
        // found in cache
        if (sp->s_dev == dev) {
            sp->s_flag |= S_LOCK;
            goto _found;
        }
    }
    // not in cache, seek an free slot
    for(sp=&mnt[0]; sp<&mnt[NMOUNT]; sp++){
        if (sp->s_dev == NODEV) {
            sp->s_dev = dev;
            read_super(sp);
            goto _found;
        }
    }
    // not found
    printf("no free mount slot");
    return -1;
_found:
    // if the inode as mount point is not a directory
    if ((ip!=NULL) && (ip->i_mode & S_IFMT)!=S_IFDIR){
        put_super(sp);
        return -1;
    }
    sp->s_imnt = ip;
    put_super(sp);
    return 0;
}

/* root lies on mnt[0], called early than any other mount, feel free 
 * to panic.
 * note: root may be changed on the execution of initialzation.
 * returns a locked super block.
 * */
int mount_root(ushort dev){
    struct buf *bp;
    struct super *sp;

    sp = read_super(dev);
    if (sp==NULL) panic("not a availible device");
    sp->s_imnt = sp->s_iroot;
    return sp;
}
