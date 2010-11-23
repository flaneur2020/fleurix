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
 * */
struct super* do_mount(ushort dev, struct inode *ip){
    struct buf *bp;
    struct super *sp;
    /*
    if ((ip==NULL) || ((ip->i_mode & S_IFMT)!=S_IFDIR)){
        panic("not an availible inode");
    }
    */
    sp = read_super(dev);
    if (sp==NULL) panic("not a availible device");
    sp->s_imnt = ip;
    return sp;
}

/* root lies on mnt[0], called quite early, feel free to panic.
 * note: root may be changed on the execution of initialzation.
 * returns a locked super block.
 * */
struct super* mount_root(ushort dev){
    struct buf *bp;
    struct super *sp;
    struct inode *ip;

    ip = iget(dev, ROOTINO);
    sp = do_mount(dev, ip);
    iput(ip);
    return sp;
}
