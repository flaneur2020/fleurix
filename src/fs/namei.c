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

/*
 * TODO: find the inode via a path string.
 * */
int namei(struct inode *ip, char *path){
    struct inode *ip, *nip;
    struct buf *bp;
    uint ino, r;
    char *tmp;

    tmp = strchr(path, '/');
    if (tmp==NULL) {
        ino = find_entry(ip, path, strlen(path));   
        return ino;
    }
    else {
        ino = find_entry(ip, path, (uint)(tmp-path));   
        nip = iget(ip->i_dev, ino);
        r = namei(nip, tmp+1);
        iput(nip);
        return r;
    }
}

/* fetch an inode in a single directory file. */
uint find_entry(struct inode* ip, char *name, uint len){
    struct inode *nip;
    struct buf *bp;
    struct dirent *dep;
    int i, j, ino=0;

    if ((ip->i_mode & S_IFMT)!=S_IFDIR) {
        current->p_error = EFAULT;
        return 0;
    }

    for(i=0; i<ip->i_size/BLK; i++){
        bn = bmap(ip, i);
        bp = bread(ip->i_dev, bn);
        dep = (struct dirent *)bp->b_addr;
        for(j=0; j<BLK/(sizeof(struct dirent)); j++) {
            if (0==strncmp(path, dep[j]->d_name, len)){
                ino = dep[j]->d_ino;
                brelse(bp);
                goto _found;
            }
        }
        brelse(bp);
    }
_found:
    return ino;
}

/*
 * TODO: this returns the inode of the directory of the specified name.
 * */
struct inode* dir_namei(char *path){
}

struct inode *find_entry(){
}
