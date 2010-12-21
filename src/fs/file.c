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

/*
 * 
 * */
int do_open(char *path, uint mode){
    struct inode *ip;
    struct file *fp;
    int fd;

    // TODO: on create a new file.
    if (mode & O_CREAT){
    }
    // an existing file.
    else {
        ip=namei(path);
        if (ip == NULL){
            iput(ip);
            current->p_error = ENFILE;
            return -1;
        }
        // TODO: check access and special files
    } 

    if ((fd=ufalloc()<0) || (fp=falloc(fd))==NULL) {
        unlk_ino(ip);
        return -1;
    }
    unlk_ino(ip);
    fp->f_mode = mode;
    fp->f_ino = ip;
    return fd;
}

int do_close(int fd){
}

/* -------------------------------------------------------------- */

/* TODO: check file access permssion */
int do_access(char *fn, uint acc){
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

    fp = current->p_ofile[fd];
    if ( fd<0 || fd>NOFILE || fp==NULL ) {
        current->p_error = ENFILE;
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
    
    fp = current->p_ofile[fd];
    if ( fd<0 || fd>NOFILE || fp==NULL) {
        current->p_error = ENFILE;
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

int do_creat(){
}

/* -------------------------------------------------------------- */

/* allocate a user file descriptor. */
int ufalloc(){
    int i;
    
    for(i=0; i<NOFILE; i++){
        if (current->p_ofile[i]==0) {
            return i;
        }
    }
    current->p_error = EMFILE;
    return -1;
}

/* allocate a file structure and attach it with an
 * user file descriptor.
 * */
struct file* falloc(int fd){
    struct file *fp;

    for(fp=&file[0]; fp<&file[NFILE]; fp++){
        if (fp->f_count==0) {
            current->p_ofile[fd] = fp;
            fp->f_count++;
            fp->f_offset = 0;
            return fp;
        }
    }
    current->p_error = ENFILE;
    return NULL;
}
