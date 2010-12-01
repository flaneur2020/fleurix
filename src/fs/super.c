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
#include <fs.h>

struct super    mnt[NMOUNT] = {0, };

/* search the mount table.
 * note: super block is meaningless until the device is mounted 
 * */
struct super* get_super(ushort dev){
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
int read_super(struct super *sp){
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
    ip = iget(sp->s_dev, ROOTINO); 
    if (ip==NULL) { 
        panic("error root inode");
        return -1;
    }
    ip->i_flag &= ~I_LOCK; // note that we do not iput.
    sp->s_iroot = ip;
    sp->s_imnt = NULL;
    return 0;
}

void write_super(struct super *sp){
}

/*****************************************************************/

/*
 * */
void unlock_super(struct super *sp){
    if (sp->s_flag & S_WANTED) {
        wakeup(sp);
    }
    sp->s_flag &= ~(S_LOCK | S_WANTED);
}

/******************************************************************/

/* 
 * remember calling this at a end of syscall. 
 * TODO: write in-core super block to disk if S_DIRTY. 
 * */
void put_super(struct super *sp){
    unlock_super(sp);
}

/* called on create a new file. 
 * */
void ialloc(){
}

/* called on remove a file 
 * */
void ifree(){
} 

/* allocate a block from a fs, via the bitmap.
 * called on increase one file's size.
 * */
void balloc(struct super *sp){
}

/* free a block */
void bfree(){
}

/* debug */
void dump_super(struct super *sp){
    printf("s_max_inode:%d\n",    sp->s_max_inode);
    printf("s_max_zone:%d\n",     sp->s_max_zone);
    printf("s_nimap_blk:%d\n",  sp->s_nimap_blk);
    printf("s_nzmap_blk:%d\n",  sp->s_nzmap_blk);
    printf("s_zone0:%d\n",        sp->s_zone0);
    printf("s_log_bz:%d\n",       sp->s_log_bz);
    printf("s_max_size:%d\n",   sp->s_max_size);
    printf("s_nzone:%d\n",      sp->s_nzone);
    printf("s_magic:%x\n",        sp->s_magic);
}
