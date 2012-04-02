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
#include <stat.h>

struct file file[NFILE];

/* -------------------------------------------------------------- */

/*
 * fcntl() performs one of the operations described below on the 
 * open file descriptor fd. The operation is determined by cmd.
 * Also there is an optional thrid argument, which associated with 
 * the second argument cmd.
 * */
/* TODO: add other commands */
int do_fcntl(int fd, uint cmd, uint arg){
    struct file *fp;
    
    fp = cu->p_ofile[fd];
    if (fd>=NOFILE || fd<0 || fp==NULL){
        syserr(EBADF);
        return -1;
    }

    switch(cmd) {
        // duplicate a fd, just as dup
        case F_DUPFD:
            return do_dup(fd);
        case F_GETFL:
            return fp->f_oflag;
        case F_SETFL:
            fp->f_oflag = arg;
            return 0;
        case F_GETFD:
            return cu->p_fdflag[fd];
        case F_SETFD:
            cu->p_fdflag[fd] = arg;
            return 0;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
        default:
            return -1;
    }
}

/* get a struct stat of an inode. a memory check is nessary. */
int do_stat(struct inode *ip, struct stat *sbuf){
    struct stat tmp;

    tmp.st_dev = ip->i_dev;
    tmp.st_ino = ip->i_num;
    tmp.st_mode = ip->i_mode;
    tmp.st_nlink = ip->i_nlink;
    tmp.st_uid = ip->i_uid;
    tmp.st_gid = ip->i_gid;
    tmp.st_rdev = ip->i_zone[0];
    tmp.st_mtime = ip->i_mtime;
    tmp.st_ctime = ip->i_ctime;
    tmp.st_atime = ip->i_atime;
    tmp.st_size = ip->i_size;

    *sbuf = tmp;
    return 0;
}

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
    if (cu->p_euid == ip->i_uid)
        m >>= 6;
    else if (cu->p_egid == ip->i_gid)
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
    if (ip->i_nlink != 0){
        syserr(EEXIST);
        iput(ip);
        return -1;
    }
    if (mode==S_IFBLK || mode==S_IFCHR) {
        ip->i_zone[0] = dev;
    }
    ip->i_mode = mode;
    ip->i_nlink = 1;
    ip->i_mtime = time();
    ip->i_uid = cu->p_euid;
    ip->i_gid = cu->p_egid;
    iupdate(ip);
    iput(ip);
    return 0;
}

/* -------------------------------------------------------------- */

