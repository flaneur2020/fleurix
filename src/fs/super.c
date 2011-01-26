#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
// 
#include <buf.h>
#include <conf.h>
//
#include <super.h>
#include <stat.h>
#include <inode.h>

struct super    mnt[NMOUNT] = {0, };

/* search the mount table.
 * note: super block is meaningless until the device is mounted 
 * */
struct super* getsp(ushort dev){
    struct super *sp;

_loop:
    for (sp=&mnt[0]; sp<&mnt[NMOUNT]; sp++){
        if (dev == sp->s_dev) {
            if (sp->s_flag & S_LOCK) {
                sp->s_flag |= S_WANTED;
                sleep(sp, PINOD);
                goto _loop;
            }
            return sp;
        }
    }
    panic("no fs");
    return NULL;
}

/*****************************************************************/

/* read a super block from disk.
 * only called on mounting, an inode in-core. (reference count remains
 * but unlocked, it's released at sys_umount)
 * */
int spload(struct super *sp){
    struct buf *bp;
    struct inode *ip;

    // read and check.
    bp = bread(sp->s_dev, 1);
    if ((bp->b_flag & B_ERROR)!=0){
        panic("disk read error");
        return -1;   
    }
    memcpy(sp, bp->b_data, sizeof(struct d_super));
    brelse(bp);
    if (sp->s_magic!=S_MAGIC) {
        panic("not an availible s_dev");
        return -1;
    }
    sp->s_imnt = NULL;
    return 0;
}

/*
 * write changes back to disk.
 * */
void spupdate(struct super *sp){
    struct inode *ip;
    struct buf *bp;
    
    bp = iget(sp->s_dev, 1);
    memcpy(bp->b_data, sp, sizeof(struct d_super));
    bwrite(bp);
    brelse(bp);
}


/* update all the fs */
void update(){
}

/* ---------------------------------------------- */

/* 
 * unlock(release) a super
 * */
void unlk_sp(struct super *sp){
    cli();
    if (sp->s_flag & S_WANTED) {
        wakeup(sp);
    }
    sp->s_flag &= ~(S_LOCK | S_WANTED);
    sti();
}

/* TODO: called on umount */
void putsp(struct super *sp){
    unlk_sp(sp);
}

/******************************************************************/

/* debug */
void dump_super(struct super *sp){
    printk("s_max_inode:%d\n",    sp->s_max_inode);
    printk("s_max_zone:%d\n",     sp->s_max_zone);
    printk("s_nimap_blk:%d\n",  sp->s_nimap_blk);
    printk("s_nzmap_blk:%d\n",  sp->s_nzmap_blk);
    printk("s_zone0:%d\n",        sp->s_data_zone0);
    printk("s_log_bz:%d\n",       sp->s_log_bz);
    printk("s_max_size:%d\n",   sp->s_max_size);
    printk("s_magic:%x\n",        sp->s_magic);
}
