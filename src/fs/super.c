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

struct super    mnt[NMOUNT] = {0, };

/* search the mount table.
 * note: super block is meaningless until the device is mounted 
 * */
struct super* get_super(ushort dev){
    struct super *sp;
    for (sp=&mnt[0]; sp<&mnt[NMOUNT]; sp++){
        if (dev == sp->s_dev) {
            return sp;
        }
    }
    panic("no fs");
    return NULL;
}

/*****************************************************************/

/* read a super block from disk into the mount table.
 * if did not got any free slot, just simply raise an error instead
 * of sleep until somebody frees like what getblk does.
 *
 * called on mounting. leave an inode in-core. (reference count remains
 * but unlocked)
 * */
struct super* read_super(ushort dev){
    struct super *sp, *tsp;
    struct buf *bp;
    struct inode *ip;

    for(sp=&mnt[0]; sp<&mnt[NMOUNT]; sp++){
        // found in cache
        if (sp->s_dev == dev) {
            sp->s_flag |= S_LOCK;
            return sp;
        }
    }
    for(sp=&mnt[0]; sp<&mnt[NMOUNT]; sp++){
        if (sp->s_dev == NODEV) {
            goto _found;
        }
    }
    // not found
    panic("no free mount slot");
    return NULL;
_found:
    // read and check.
    bp = bread(dev, 1);
    memcpy(sp, bp->b_addr, sizeof(struct d_super));
    brelse(bp);
    if (sp->s_magic!=S_MAGIC) {
        panic("not an availible dev");
        return NULL;
    }
    sp->s_dev = dev;
    ip = iget(dev, ROOTINO); 
    if (ip==NULL) panic("error root inode"); 
    ip->i_flag &= ~I_LOCK; // note that we do not puti.
    sp->s_iroot = ip;
    sp->s_imnt = NULL;
    return sp;
}

void write_super(struct super *sp){
}

/*****************************************************************/

/* remember calling this at a end of syscall.
 * */
void unlock_super(struct super *sp){
    sp->s_flag &= ~S_LOCK;
}

/******************************************************************/

/* write in-core super block to disk. */
void put_super(struct super *sp){
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
