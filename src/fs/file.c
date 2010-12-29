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
    return 1;
}

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
            ip->i_nlinks++;
            iupdate(ip);
        }
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
    uint nr;
    struct file *fp;

    if ((fd>NOFILE) || (fd<0)){
        syserr(EBADF);
        return -1;
    }

    nr = cu->p_ofile[fd];
    if (nr>NFILE || nr<0) {
        syserr(EBADF);
        return -1;
    }
    cu->p_ofile[fd] = 0;
    fp = &file[nr];
    iput(fp->f_ino);
    fp->f_count--;
    if (fp->f_count <= 0) {
        fp->f_count = 0;
        fp->f_flag = 0;
        fp->f_offset = 0;
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
}

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
    if (name==NULL) {
        name = path2;
    }
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
    if (name==NULL) {
        name = path;
    }
    ino = unlink_entry(dip, name, strlen(name));
    ip = iget(dip->i_dev, ino);
    ip->i_nlinks--;
    iput(ip);
    iput(dip);
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
            fp->f_flag = 0;
            return fp;
        }
    }
    cu->p_error = ENFILE;
    return NULL;
}
