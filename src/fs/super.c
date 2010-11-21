#include <param.h>
#include <x86.h>
#include <buf.h>
#include <super.h>

/* */
void get_fs(int dev){
    bread(dev, 3);
}

void put_fs(struct fs *fsp){
}

/* allocate a block from a fs */
void balloc(struct fs *fs){
}

/* free a block */
void bfree(){
}
