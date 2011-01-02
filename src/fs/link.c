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
#include <file.h>

/*
 * link path1 to path2. 
 * path1 is existing yet, create a new directory entry for path2, 
 * and increase the reference count of path1's inode.
 * */
int do_link(char *path1, char *path2){
    struct inode *tip, *dip;
    int ino, r;
    char *name;

    // TODO: check permission
    name = strrchr(path2, '/');
    name = (name==NULL) ? path2: (name+1);
    if (strlen(name)==0 || strlen(name) >= NAMELEN) {
        return -ENOENT;
    }
    if (strcmp(name, ".")==0 || strcmp(name, "..")==0){
        return -EPERM;
    }

    // get the inode number.
    tip = namei(path1, 0);
    if (tip==NULL) {
        iput(tip);
        return -ENOENT;
    }
    ino = tip->i_num;
    tip->i_nlinks++;
    iupdate(tip);
    unlk_ino(tip); // the next namei may deadlock. so unlock it.

    // get the inode of the target's directory
    dip = namei_parent(path2);
    // if entry already exists, error.
    r = find_entry(dip, name, strlen(name));
    if (r!=0) {
        // undo something 
        lock_ino(tip);
        tip->i_nlinks--;
        iupdate(tip);
        iput(tip);
        iput(dip);
        return -EEXIST;
    }
    // do link
    lock_ino(tip);
    r = link_entry(dip, name, strlen(name), tip->i_num);
    if (r==0) {
        panic("bad link. ");
    }
    iupdate(tip); // ref count increased
    iput(tip);
    iput(dip);
    return 0;
}

/*
 * remove a link to a file.
 * this file should not be a directory.
 * returns 0 on success. 
 * */
int do_unlink(char *path){
    struct inode *ip, *dip;
    int ino;
    char *name;

    // get the inode of the target's directory
    name = strrchr(path, '/');
    name = (name==NULL) ? path: (name+1);
    // on path=='/' 
    if (strlen(name)==0 || strlen(name) >= NAMELEN){
        syserr(EPERM);
        return -1;
    }
    if (strcmp(name, ".")==0 || strcmp(name, "..")==0){
        syserr(EPERM);
        return -1;
    }

    dip = namei_parent(path);
    ino = unlink_entry(dip, name, strlen(name));
    // entry not found
    if (ino<=0) {
        iput(dip);
        syserr(EPERM);
        return -1;
    }
    ip = iget(dip->i_dev, ino);
    // can't unlink a directory, undo something.
    if ((ip->i_mode & S_IFMT) == S_IFDIR) {
        link_entry(dip, name, strlen(name));
        iput(ip);
        iput(dip);
        syserr(EPERM);
        return -1;
    }
    ip->i_nlinks--;
    iput(ip);
    iput(dip);
    return 0;
}

