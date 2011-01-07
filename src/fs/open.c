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
#include <file.h>

struct file file[NFILE];

/* ------------------------------------------------------------- */

/*
 * open a file. flag indicated opened type like O_RDONLY, O_TRUNC, O_CREAT and blah. And 
 * mode only used in the O_CREAT scenary, indicated the file (inode) type.
 *
 * each proc has got one user file table(p_ofile[NOFILE]), it's each entry is also a number,
 * indicated the number in the system file table(file[NFILE]). when user opened a file, it 
 * first allocate a user file table entry(aka. file descriptor), then attach it with a system
 * file table entry. It's reference count is increased in fork() or dup().
 * */
int do_open(char *path, uint flag, uint mode){
    struct inode *ip;
    struct file *fp;
    int fd;

    // on create a new file.
    if (flag & O_CREAT){
        ip = namei(path, 1);
        // if file is not existing yet.
        if (ip->i_nlinks==0) {
            ip->i_mode = mode;
            ip->i_uid = cu->p_uid;
            ip->i_gid = cu->p_gid;
            ip->i_time = time();
            ip->i_nlinks = 1;
            iupdate(ip);
        }
    }
    // an existing file.
    else {
        ip = namei(path, 0);
        if (ip == NULL){
            syserr(ENFILE);
            return -1;
        }
        // TODO: check access and special files
    } 
    if (((fd=ufalloc())<0) || (fp=falloc(fd))==NULL) {
        iput(ip);
        return -1;
    }
    if (flag & O_TRUNC){
        itrunc(ip);
    }
    unlk_ino(ip);
    fp->f_flag = flag;
    fp->f_ino = ip;
    return fd;
}

/*
 * close a user file discriptor.
 * remove the entry in the user file table, and decrease the entry in system 
 * file table's ref count. and iput the inode.
 * */
int do_close(int fd){
    struct file *fp;

    if ((fd>NOFILE) || (fd<0)){
        syserr(EBADF);
        return -1;
    }

    fp = cu->p_ofile[fd];
    if (fp==NULL || fp>&file[NFILE] || fp<&file[0]) {
        syserr(EBADF);
        return -1;
    }
    cu->p_ofile[fd] = NULL;
    iput(fp->f_ino);
    fp->f_count--;
    if (fp->f_count <= 0) {
        fp->f_count = 0;
        fp->f_flag = 0;
        fp->f_offset = 0;
    }
    return 0;
}

/* duplicate a file descriptor, dup2 */
int do_dup(int fd){
    struct file *fp;
    int newfd;

    fp = cu->p_ofile[fd];
    if (fd>=NOFILE || fp==NULL){
        syserr(EBADF);
        return -1;
    }
    if ((newfd=ufalloc())<0) {
        return -1;
    }
    fp->f_ino->i_count++;
    cu->p_ofile[newfd] = fp;
    return newfd;
}

/* close the newfd. */
int do_dup2(int fd, int newfd){
    struct file *fp;

    fp = cu->p_ofile[fd];
    if (fd>=NOFILE || fp==NULL){
        syserr(EBADF);
        return -1;
    }

    do_close(newfd);
    fp->f_ino->i_count++;
    cu->p_ofile[newfd] = fp;
    return newfd;
}

/* -------------------------------------------------------------- */

/* allocate a user file descriptor. */
int ufalloc(){
    int i;
    
    for(i=0; i<NOFILE; i++){
        if (cu->p_ofile[i]==NULL) {
            return i;
        }
    }
    syserr(ENFILE);
    return -1;
}

/* allocate a file structure and attach it with an
 * user file descriptor.
 * */
struct file* falloc(int fd){
    struct file *fp;

    for(fp=&file[0]; fp<&file[NFILE]; fp++){
        if (fp->f_count==0) {
            fp->f_count = 1;
            cu->p_ofile[fd] = fp;
            fp->f_offset = 0;
            fp->f_flag = 0;
            return fp;
        }
    }
    syserr(EMFILE);
    return NULL;
}
