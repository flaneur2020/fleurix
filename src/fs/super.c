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

/* */
struct super* get_super(short dev){
    struct mount *mp;
    for (mp=&mnt[0]; mp<&mnt[NMOUNT]; mp++){
        if (dev==mp->m_dev) {
            return &(mp->m_super);
        }
    }
    return NULL;
}

void put_fs(struct super *sp){
}

/* allocate a block from a fs */
void balloc(struct super *sp){
}

/* free a block */
void bfree(){
}
