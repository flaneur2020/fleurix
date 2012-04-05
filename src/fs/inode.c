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

struct inode inode[NINODE];

/*
 * this file indicated some operation on inode. such as
 * iget -> get an locked inode.
 * iput -> release an locked inode.
 * iload -> load an inode from disk.
 * iupdate -> write changes back to disk.
 * */

/* get an (locked) inode via an number 
 * if the inode is in cache, return it right now.
 * return NULL on error.
 * note1: you may compare this code with getblk, the idea here
 * is common used among everywhere on resource allocation.
 * it returns ONLY locked inode just as B_BUSY in getblk, just 
 * to prevent other processes' accessing within one syscall.
 *
 * note2: though there is still some differences between getblk,
 * put an eye on *reference count*, lock won't stays long(within 
 * one syscall, like the open routine, which unlock the inode at
 * last), though *reference count* ramains set between syscalls
 * to prevent the kernel from reallocating active in-core inode.
 *
 * */
struct inode* iget(ushort dev, uint num){
    struct inode *ip;
    struct super *sp;

_loop:
    for(ip=&inode[0]; ip<&inode[NINODE]; ip++){
        if (ip->i_dev==dev && ip->i_num==num) {
            // if found but locked.
            if (ip->i_flag & I_LOCK) {
                ip->i_flag |= I_WANTED;
                sleep((uint)ip, PINOD);
                goto _loop;
            }
            // if this is an mount point, redirect to the mount
            // target's root inode.
            if (ip->i_flag & I_MOUNT) {
                for (sp=&mnt[0]; sp<&mnt[NMOUNT]; sp++){
                    if (ip == sp->s_imnt){
                        // iget(sp->s_dev, ROOTINO);
                        dev = sp->s_dev;
                        num = ROOTINO;
                        goto _loop;
                    }
                }
            }
            // if found straightly
            ip->i_count++;
            ip->i_flag |= I_LOCK;
            return ip;
        }
    }
    // not cached, so seek one free slot
    for(ip=&inode[0]; ip<&inode[NINODE]; ip++){
        // time to read disk
        if (ip->i_count==0) {
            ip->i_dev = dev;
            ip->i_num = num;
            ip->i_flag = I_LOCK;
            ip->i_count++;
            iload(ip);
            return ip;
        }
    }
    // if not found (no free slot) 
    printk("inode table overflow.\n");
    return NULL;
}

/* 
 * release an inode.
 * decrease the reference count, write updates to disk if nessary.
 * also check the link count, if zero, trucate it.
 * */
void iput(struct inode *ip){
    ushort dev;

    if(ip==NULL)
        panic("bad struct inode*");
    ip->i_flag |= I_LOCK;
    if (ip->i_count > 0) {
        ip->i_count--;
    }
    if (ip->i_count==0){
        if (ip->i_nlink==0) {
            itrunc(ip);
            ifree(ip->i_dev, ip->i_num);
        }
        // if it's a device file, the dev number is stored in zone[0].
        dev = ip->i_zone[0];
        switch (ip->i_mode & S_IFMT) {
            case S_IFBLK:
                (*bdevsw[MAJOR(dev)].d_close)(dev);
                break;
            case S_IFCHR:
                (*cdevsw[MAJOR(dev)].d_close)(dev);
                break;
        }
        iupdate(ip);
        ip->i_flag = 0;
        ip->i_num = 0;
    }
    unlk_ino(ip);
}

/***************************************************/

/* load a inode from disk 
 * */
int iload(struct inode *ip){
    struct super *sp;
    struct d_inode *itab; /* note this is an d_inode, 32 bytes. */
    struct buf *bp;

    sp = getsp(ip->i_dev);
    if (sp==NULL){
        panic("error on reading a super");
    }
    // get the blk number where this inode lies in.
    bp = bread(ip->i_dev, IBLK(sp, ip->i_num));
    if (bp->b_flag & B_ERROR) {
        panic("error on reading an inode");
    }
    itab = (struct d_inode*)bp->b_data;
    memcpy(ip, &itab[(ip->i_num-1)%IPB], sizeof(struct d_inode));
    brelse(bp);
    return 0;
}

/* write changes back to disk if nessary. */
void iupdate(struct inode *ip){
    struct super *sp;
    struct d_inode *itab;
    struct buf *bp;

    /*
    if ((ip->i_flag & I_DIRTY)==0){
        return;
    }
    */

    sp = getsp(ip->i_dev);
    if (sp==NULL){
        panic("bad super");
    }
    // get the blk number where this inode lies in.
    bp = bread(ip->i_dev, IBLK(sp, ip->i_num));
    if (bp->b_flag & B_ERROR) {
        panic("error on reading an inode");
    }
    itab = (struct d_inode*)bp->b_data;
    // which different from above.
    memcpy(&itab[(ip->i_num-1)%IPB], ip, sizeof(struct d_inode));
    ip->i_flag &= ~I_DIRTY;
    bwrite(bp);
    brelse(bp);
    return;
}

/*************************************************************/

void lock_ino(struct inode *ip){
    while(ip->i_flag & I_LOCK){
        sleep((uint)ip, PINOD);
    }
    ip->i_flag |= I_LOCK;
}

/* remember this just free with malloc. */
void unlk_ino(struct inode *ip){
    cli();
    if (ip->i_flag & I_WANTED) {
        wakeup((uint)ip);
    }
    ip->i_flag &= ~(I_LOCK | I_WANTED);
    sti();
}

/*************************************************************/

void dump_inode(struct inode *ip){
    printk("i_mode:%x\n", ip->i_mode);
    printk("i_uid:%x\n", ip->i_uid);
    printk("i_size:%x\n", ip->i_size);
    printk("i_mtime:%x\n", ip->i_mtime);
    printk("i_gid:%x\n", ip->i_gid);
    printk("i_nlink:%x\n", ip->i_nlink);
    printk("i_count:%x\n", ip->i_count);
    printk("i_dev:%x\n", ip->i_dev);
    printk("i_num:%x\n", ip->i_num);
    printk("i_flag:%x\n", ip->i_flag);
    int i=0;
    for(i=0;i<9;i++){
        printk("i_zone[%d]: %x\n", i, ip->i_zone[i]);
    }

    /*
    uint nr;
    struct buf *bp;

    bp = bread(rootdev, nr);
    dump_buf(bp);
    brelse(bp);
    printk("\n"); */
}
