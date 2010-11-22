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

/* search the mount table.
 * note: super block is meaningless until the device is mounted 
 * */
struct super* get_super(short dev){
    struct mount *mp;
    for (mp=&mnt[0]; mp<&mnt[NMOUNT]; mp++){
        if (dev==mp->m_dev) {
            return &(mp->m_super);
        }
    }
    return NULL;
}

/* write in-core super block to disk. */
void put_super(struct super *sp){
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
    printf("max_inode:%d\n",    sp->s_max_inode);
    printf("max_zone:%d\n",     sp->s_max_zone);
    printf("s_nimap_blk:%d\n",  sp->s_nimap_blk);
    printf("s_nzmap_blk:%d\n",  sp->s_nzmap_blk);
    printf("zone0:%d\n",        sp->s_zone0);
    printf("log_bz:%d\n",       sp->s_log_bz);
    printf("s_max_size:%d\n",   sp->s_max_size);
    printf("s_nzone:%d\n",      sp->s_nzone);
    printf("magic:%x\n",        sp->s_magic);
}
