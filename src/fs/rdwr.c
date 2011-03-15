#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
// 
#include <buf.h>
#include <tty.h>
#include <conf.h>
//
#include <super.h>
#include <inode.h>
#include <file.h>
#include <stat.h>

struct file file[NFILE];
/*
 * read a file via a file descriptor.
 * returns -1 on EOF and error
 *
 * TODO: consider special file later.
 * */
int do_read(int fd, char *buf, int cnt){
    struct file *fp;
    struct inode *ip;
    uint dev;
    int r;

    fp = cu->p_ofile[fd];
    if ( fd<0 || fd>NOFILE || fp==NULL ) {
        syserr(ENFILE);
        return -1;
    }
    // on write only
    if (fp->f_oflag & O_WRONLY) {
        syserr(EBADF);
        return -1;
    }

    ip = fp->f_ino;
    lock_ino(fp->f_ino);
    switch(ip->i_mode & S_IFMT) {
        case S_IFBLK:
            // TODO;
            break;
        case S_IFCHR:
            dev = ip->i_dev;
            r = (*cdevsw[MAJOR(dev)].d_read)(dev, buf, cnt);
            break;
        case S_IFDIR:
        case S_IFREG:
        default:
            r = readi(ip, buf, fp->f_offset, cnt);
    }
    if (r < 0){
        unlk_ino(ip);
        return -1;
    }
    fp->f_offset += cnt;
    unlk_ino(ip);
    return r;
}

int do_write(int fd, char *buf, int cnt){
    struct file *fp;
    struct inode *ip;
    int r, off;
    uint dev;
    
    fp = cu->p_ofile[fd];
    if ( fd<0 || fd>NOFILE || fp==NULL) {
        syserr(ENFILE);
        return -1;
    }
    // on read only
    if (fp->f_oflag & O_RDONLY) {
        syserr(EBADF);
        return -1;
    }
    //
    if (fp->f_oflag & O_APPEND) {
        off = fp->f_ino->i_size;
    }
    else {
        off = fp->f_offset;
    }
    //
    ip = fp->f_ino;
    lock_ino(ip);
    switch(ip->i_mode & S_IFMT) {
    case S_IFBLK:
        // TODO: 
        break;
    case S_IFCHR:
        dev = ip->i_dev;
        r = (*cdevsw[MAJOR(dev)].d_write)(dev, buf, cnt);
        break;
    case S_IFDIR:
    case S_IFREG:
    default:
        r = writei(ip, buf, off, cnt);
    }
    if (r < 0){
        unlk_ino(ip);
        return -1;
    }
    fp->f_offset = off + cnt;
    unlk_ino(ip);
    return r;
}

/*
 * reposition the opened file asscociated with the file descriptor fd 
 * to the argument off, according to the directive whence below:
 *  SEEK_SET: set the offset as the argument off
 *  SEEK_CUR: set the offset to its current location plus off bytes.
 *  SEEK_END: set the offset to its size plus off bytes.
 * */
int do_lseek(uint fd, int off, int whence){
    struct file *fp;

    fp = cu->p_ofile[fd];
    if ((fd >= NOFILE) || (fp==NULL) || (fp->f_ino==NULL)) {
        syserr(EBADF);
        return -1;
    }
    if (fp->f_ino->i_mode & S_IFIFO) {
        syserr(ESPIPE);
        return -1;
    }

    if (whence==SEEK_SET) {
        if (off < 0) 
            goto _einval;
        fp->f_offset = off;
    }
    if (whence==SEEK_CUR) { 
        if (fp->f_offset+off < fp->f_offset) 
            goto _einval;
        fp->f_offset += off;
    }
    if (whence==SEEK_END) {
        if (fp->f_ino->i_size+off < fp->f_ino->i_size)
            goto _einval;
        fp->f_offset = fp->f_ino->i_size + off;
    }
    return fp->f_offset;

_einval:
    syserr(EINVAL);
    return -1;
}

