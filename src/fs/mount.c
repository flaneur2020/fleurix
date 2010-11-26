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

struct super *rootsp = NULL;

/* load a super block in-core. and associate with an in-core inode (locked).
 *
 * if did not got any free slot, just simply raise an error instead
 * of sleep until somebody frees like what getblk does.
 * if dev==rootdev, set a pointer rootsp for a quicker access.
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
    // not in cache, seek an free slot and read the disk.
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
    if (dev==rootdev) {
        rootsp = sp;
    }
    sp->s_imnt = ip;
    put_super(sp);
    return 0;
}


/*
 * TODO: umount();
 * unmount an device.
 * */
int do_umount(ushort dev){

}
