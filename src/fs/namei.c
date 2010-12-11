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


/* 
 * fetch an inode number from a single directory file (via a locked inode). 
 * */
uint find_entry(struct inode* ip, char *name, uint len){
    struct inode *nip;
    struct buf *bp;
    struct dirent *dep;
    int i, j, bn=0, ino=0;

    if ((ip->i_mode & S_IFMT)!=S_IFDIR) {
        current->p_error = EFAULT;
        return 0;
    }

    for(i=0; i<ip->i_size/BLK+1; i++){
        bn = bmap(ip, i);
        bp = bread(ip->i_dev, bn);
        dep = (struct dirent *)bp->b_data;
        for(j=0; j<BLK/(sizeof(struct dirent))+1; j++) {
            if (0==strncmp(name, dep[j].d_name, len)){
                ino = dep[j].d_ino;
                brelse(bp);
                return ino;
            }
        }
        brelse(bp);
    }
    return 0;
}

/*
 * returns a locked inode.
 * take an eye on dead lock.
 * */
struct inode* do_namei(char *path){
    struct inode *wip=NULL, *cdp=NULL;
    uint ino, offset;
    char* tmp;

    // if path starts from root
    // note if p_cdir==NULL, it's an early initialized process, 
    // set it's root directory here.
    if (*path == '/') {
        wip = iget(rootdev, ROOTINO);
        current->p_cdir = wip;
    }
    else {
        cdp = current->p_cdir;
        wip = iget(cdp->i_dev, cdp->i_num);
    }

    // while there is more path name with '/'
    while (*path != '\0') {
        if (*path=='/'){
            path++;
            continue;
        }
        // if working inode is root and componet is ".."
        if ((wip->i_num==ROOTINO) && (strncmp(path, "..", 2)==0)) {
            continue;
        }
        tmp = strchr(path, '/');
        offset = (tmp==NULL) ? strlen(path): (tmp-path);
        ino = find_entry(wip, path, offset);
        if (ino == 0){
            return NULL;
        }
        iput(wip);
        wip = iget(wip->i_dev, ino);
        path += offset;
    }
    return wip;
}
