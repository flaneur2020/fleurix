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
 * */
int do_link(char *path1, char *path2){
    struct inode *tip, *dip;
    int ino, r;
    char *name;

    // TODO: check permission

    // get the inode number.
    tip = namei(path1, 0);
    if (tip==NULL) {
        syserr(ENOENT);
        iput(tip);
        return -1;
    }
    ino = tip->i_num;
    tip->i_nlinks++;
    unlk_ino(tip); // the next namei may deadlock. so unlock it.

    // get the inode of the target's directory
    dip = namei_parent(path2);
    name = strrchr(path2, '/');
    name = (name==NULL) ? path2: (name+1);
    // if entry already exists, error.
    r = find_entry(dip, name, strlen(name));
    if (r!=0) {
        syserr(EEXIST);
        iput(tip);
        iput(dip);
        return -1;
    }
    // do link
    lock_ino(tip);
    r = link_entry(dip, name, strlen(name), tip->i_num);
    if (r==0) {
        panic(" bad link.");
    }
    iupdate(tip); // ref count increased
    iput(tip);
    iput(dip);
    return 0;
}

/*
 * remove.
 * returns 0 on success. Otherwise returns -1 and errno 
 * set to indicate the error.
 * */
int do_unlink(char *path){
    struct inode *ip, *dip;
    int ino;
    char *name;

    // get the inode of the target's directory
    dip = namei_parent(path);
    name = strrchr(path, '/');
    name = (name==NULL) ? path: (name+1);
    ino = unlink_entry(dip, name, strlen(name));
    // entry not found
    if (ino==0) {
        iput(dip);
        syserr(ENOENT);
        return -1;
    }
    ip = iget(dip->i_dev, ino);
    ip->i_nlinks--;
    iput(ip);
    iput(dip);
    return 0;
}

