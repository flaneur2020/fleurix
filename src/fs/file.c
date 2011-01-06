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

struct file file[NFILE];

/* -------------------------------------------------------------- */

/* check file access and permssion. mode is R_OK, W_OK and X_OK. 
 * In the case of write, the read-only status of the file system is
 * checked.
 * The super user is granted all permissions except for EXEC where
 * at least one of the EXEC bits must be on.
 *
 * returns 0 on OK and -1 on fail.
 * */
int do_access(struct inode *ip, uint mode){
    struct super *sp;
    uint m;
    
    if (ip==NULL) {
        syserr(EACCES);
        return -1;
    }

    if (mode & W_OK) {
        // fs is readonly
        sp = getsp(ip->i_dev);
        unlk_sp(sp);
        if (sp->s_flag & S_RDONLY) {
            syserr(EROFS);
            return -1;
        }
        // this inode is running.
        if (ip->i_flag & I_TEXT) {
            return -1;
        }
    }
    // super user is granted all permissions. but X_OK needs at least one bit on.
    if (suser()){
        if (mode & X_OK) {
            if ((ip->i_mode & (X_OK|X_OK<<3|X_OK<<6))){
                syserr(EACCES);
                return -1;
            }
            return 0;
        }
    }
    //
    m = ip->i_mode & 0777;
    if (cu->p_uid == ip->i_uid)
        m >>= 6;
    else if (cu->p_gid == ip->i_gid)
        m >>= 3;
    // 
    if ((m & 07 & mode)==mode) {
        return 0;
    }
    syserr(EACCES);
    return -1;
}

/* -------------------------------------------------------------- */

/*
 * read a file via a file descriptor.
 * returns -1 on EOF and error
 *
 * TODO: consider special file later.
 * */
int do_read(int fd, char *buf, int cnt){
    struct file *fp;
    struct inode *ip;
    int r;

    fp = cu->p_ofile[fd];
    if ( fd<0 || fd>NOFILE || fp==NULL ) {
        syserr(ENFILE);
        return -1;
    }
    // TODO: special files
    lock_ino(fp->f_ino);
    r = readi(fp->f_ino, buf, fp->f_offset, cnt);
    if (r < 0){
        unlk_ino(fp->f_ino);
        return -1;
    }
    fp->f_offset += cnt;
    unlk_ino(fp->f_ino);
    return r;
}

int do_write(int fd, char *buf, int cnt){
    struct file *fp;
    struct inode *ip;
    int r, off;
    
    fp = cu->p_ofile[fd];
    if ( fd<0 || fd>NOFILE || fp==NULL) {
        syserr(ENFILE);
        return -1;
    }
    if (fp->f_flag & O_APPEND) {
        off = fp->f_ino->i_size;
    }
    else {
        off = fp->f_offset;
    }
    // TODO: special files
    lock_ino(fp->f_ino);
    r = writei(fp->f_ino, buf, off, cnt);
    if (r < 0){
        unlk_ino(fp->f_ino);
        return -1;
    }
    fp->f_offset = off + cnt;
    unlk_ino(fp->f_ino);
    return r;
}

/* -------------------------------------------------------------- */

/* 
 * creat a new file if not existing yet. if existed, truncate it.
 * this routine also returns a file descriptor just like do_open().
 * */
int do_creat(char *path, int mode){
    return do_open(path, O_CREAT | O_TRUNC, mode);
}

/* 
 * */
int do_mknod(char *path, int mode, ushort dev){
    struct inode *ip;

    ip = namei(path, 1);
    // if existing
    if (ip==NULL) {
        syserr(ENOENT);
        iput(ip);
        return -1;
    }
    if (ip->i_nlinks != 0){
        syserr(EEXIST);
        iput(ip);
        return -1;
    }
    if (mode==S_IFBLK || mode==S_IFCHR) {
        ip->i_zone[0] = dev;
    }
    ip->i_mode = mode;
    ip->i_nlinks = 1;
    ip->i_time = time();
    ip->i_uid = cu->p_uid;
    ip->i_gid = cu->p_gid;
    iupdate(ip);
    iput(ip);
    return 0;
}

/* -------------------------------------------------------------- */

