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

struct super *rootsp = NULL;

/* load a super block in-core. and associate with an in-core inode (locked).
 * on mounting the root filesystem, do_mount(rootdev, NULL);
 *
 * if did not got any free slot, just simply raise an error instead
 * of sleep until somebody frees like what getblk does.
 * if dev==rootdev, set a pointer rootsp for a quicker access.
 *
 * */
struct super* do_mount(ushort dev, struct inode *ip){
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
            spload(sp);
            goto _found;
        }
    }
    // not found
    printk("no free mount slot");
    return NULL;

_found:
    // if the inode as mount point is not a directory
    if ((ip!=NULL) && (ip->i_mode & S_IFMT)!=S_IFDIR){
        putsp(sp);
        return NULL;
    }
    if (dev==rootdev) {
        rootsp = sp;
    }
    if (ip!=NULL)
        ip->i_count++;
    sp->s_imnt = ip;
    unlk_sp(sp);
    return sp;
}

/*
 * umount();
 * unmount an device.
 * */
int do_umount(ushort dev){
    struct super *sp;
    struct inode *ip;

    sp = getsp(dev);
    for(ip=&inode[0]; ip<&inode[NINODE]; ip++){
        if (ip->i_num!=0 && ip->i_dev==dev) {
            syserr(EBUSY);
            return -1;
        }
    }
    spupdate(sp);
    sp->s_dev = 0;
    sp->s_flag = 0;
    iput(sp->s_imnt);
    return 0;
}
