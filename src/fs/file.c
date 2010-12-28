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

/* TODO: check file access and permssion */
int do_access(char *fn, uint acc){
}

/* ------------------------------------------------------------- */
/*
 * 
 * */
int do_open(char *path, uint flag, uint mode){
    struct inode *ip;
    struct file *fp;
    int fd;

    // TODO: on create a new file.
    if (flag & O_CREAT){
    }
    // an existing file.
    else {
        ip = namei(path, 0);
        if (ip == NULL){
            iput(ip);
            syserr(ENFILE);
            return -1;
        }
        // TODO: check access and special files
    } 
    if ((fd=ufalloc()<0) || (fp=falloc(fd))==NULL) {
        unlk_ino(ip);
        return -1;
    }
    if (flag & O_TRUNC){
        itrunc(ip);
    }
    unlk_ino(ip);
    fp->f_mode = ip->i_mode;
    fp->f_flag = flag;
    fp->f_ino = ip;
    return fd;
}

/* TODO: have a reference with <the unix os design> */
int do_close(int fd){
    uint nr;
    struct file *fp;

    if ((fd>NOFILE) || (fd<0)){
        syserr(EBADF);
        return -1;
    }

    nr = cu->p_ofile[fd];
    cu->p_ofile[fd] = 0;
    fp = &file[nr];
    if (fp == NULL) {
        syserr(EBADF);
        return -1;
    }
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
        cu->p_error = ENFILE;
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
        cu->p_error = ENFILE;
        return -1;
    }
    if (fp->f_mode & O_APPEND) {
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

int do_creat(char *path, int mode){
    return do_open(path, O_CREAT | O_TRUNC, mode);
}

int do_mknod(){
}

/*
 * create a new link (directory entry) for the existing file, path1.
 * */
int do_link(char *path1, char *path2){
}

/*
 * remove  
 * returns 0 on success. Otherwise returns -1 and errno 
 * set to indicate the error.
 * */
int do_unlink(char *path){
    struct inode *ip;
    char *name;

    ip = namei(path, 0);
    if (ip == NULL) {
        syserr(ENOENT);
        return -1;
    }

    ip->i_nlinks--;
    iput(ip);
    return 0;
}

/* -------------------------------------------------------------- */

/* allocate a user file descriptor. */
int ufalloc(){
    int i;
    
    for(i=0; i<NOFILE; i++){
        if (cu->p_ofile[i]==0) {
            return i;
        }
    }
    syserr(EMFILE);
    return -1;
}

/* allocate a file structure and attach it with an
 * user file descriptor.
 * */
struct file* falloc(int fd){
    struct file *fp;

    for(fp=&file[0]; fp<&file[NFILE]; fp++){
        if (fp->f_count==0) {
            cu->p_ofile[fd] = fp;
            fp->f_count++;
            fp->f_offset = 0;
            return fp;
        }
    }
    cu->p_error = ENFILE;
    return NULL;
}
