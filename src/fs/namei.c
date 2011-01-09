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
#include <stat.h>


/* 
 * fetch an inode number from a single directory file (via a locked inode, make sure
 * it's an directory).
 * returns 0 on fail.
 * NOTE: minix v1's direntry is aligned with a 32bytes boundary.
 * */
uint find_entry(struct inode* dip, char *name, uint len){
    struct buf *bp;
    struct dirent *dep;
    int i, j, bn=0, ino=0;

    if ((dip->i_mode & S_IFMT)!=S_IFDIR) {
        syserr(EFAULT);
        return 0;
    }
    len = min(len, NAMELEN);

    for(i=0; i<dip->i_size/BSIZE+1; i++){
        bn = bmap(dip, i, 0);
        bp = bread(dip->i_dev, bn);
        dep = (struct dirent *)bp->b_data;
        for(j=0; j<BSIZE/(sizeof(struct dirent))+1; j++) {
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
 * unlink a entry, like above.
 * */
int unlink_entry(struct inode *dip, char *name, int len){
    struct buf *bp;
    struct dirent *dep;
    int i, j, bn=0, ino=0;

    if ((dip->i_mode & S_IFMT)!=S_IFDIR) {
        syserr(EFAULT);
        return 0;
    }
    len = min(len, NAMELEN);

    for(i=0; i<dip->i_size/BSIZE+1; i++){
        bn = bmap(dip, i, 0);
        bp = bread(dip->i_dev, bn);
        dep = (struct dirent *)bp->b_data;
        for(j=0; j<BSIZE/(sizeof(struct dirent))+1; j++) {
            if (0==strncmp(name, dep[j].d_name, len)){
                ino = dep[j].d_ino;
                dep[j].d_ino = 0;
                bwrite(bp);
                brelse(bp);
                return ino;
            }
        }
        brelse(bp);
    }
    return 0;
}

/*
 * assign a new directory entry with a given inode. 
 * note: this routine do NOT check the existence of the given name,
 * and i_nlinks is not adjusted here.
 * */
uint link_entry(struct inode *dip, char *name, uint len, uint ino){
    struct buf *bp;
    struct inode *ip;
    struct dirent de;
    int i, r, off;

    if ((dip->i_mode & S_IFMT)!=S_IFDIR) {
        syserr(ENOTDIR);
        return 0;
    }
    len = min(len, NAMELEN);

    for (off=0; off < dip->i_size; off+=sizeof(struct dirent)){
        r = readi(dip, &de, off, sizeof(struct dirent));
        if (r != sizeof(struct dirent)){
            panic("bad read dir ino");
        }
        if (de.d_ino == 0) {
            break;
        }
    }
    strncpy(de.d_name, name, len+1);
    de.d_ino = ino;
    r = writei(dip, &de, off, sizeof(struct dirent));
    if (r != sizeof(struct dirent)){
        panic("bad inode");
    }
    return ino;
}

/*
 * returns a locked inode. NULL on error.
 * */
struct inode* _namei(char *path, uchar creat, uchar parent, char **name){
    struct inode *wip=NULL, *cdp=NULL;
    ushort dev, ino, len;
    char* tmp;

    // if path starts from root
    // note if p_cdir==NULL, it's an early initialized process, 
    // set it's root directory here.
    if (*path == '/') {
        wip = iget(rootdev, ROOTINO);
        cu->p_cdir = wip;
    }
    else {
        cdp = cu->p_cdir;
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
        // wip must be a directory
        if ((wip->i_mode & S_IFMT)!=S_IFDIR) {
            iput(wip);
            syserr(EISDIR);
            return NULL;
        }
        // TODO: check access
        tmp = strchr(path, '/');
        // if got the parent inode.
        if (tmp==NULL) {
            if (parent) {
                *name = path;
                return wip;
            }
            len = strlen(path);
            dev = wip->i_dev;
        }
        else {
            len = tmp-path;
        }
        ino = find_entry(wip, path, len);
        // if not found
        if (ino <= 0){
            if (creat==0) {
                iput(wip);
                return NULL;
            }
            // file is not found and creat is set, assign a new inode
            ino = ialloc(wip->i_dev);
            wip->i_flag |= I_LOCK;
            // what a fuck
            link_entry(wip, path, len, ino);
        }
        iput(wip);
        wip = iget(wip->i_dev, ino);
        path += len;
    }
    return wip;
}

struct inode* namei(char *path, uchar creat){
    return _namei(path, creat, 0, NULL);
}

struct inode* namei_parent(char *path, char **name){
    return _namei(path, 0, 1, name);
}
