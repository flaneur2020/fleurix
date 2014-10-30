#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>

#include <buf.h>
#include <inode.h>
#include <super.h>
#include <file.h>
#include <stat.h>

/* syscalls on fs */

/*
 * WARNING: DANGEROUS here! While passing data between kernel
 * and user, a verification is nesscary. Just make a simulation
 * what the hardware does in user space, or it may cause corruption,
 * even overlaps the kernel memory, be careful.
 *
 * */

/* int access(char *path, int mode); */
int sys_access(struct trap *tf){
    char *path = (char*)tf->ebx;
    int mode = tf->ecx;
    struct inode *ip;

    if (vm_verify(path, MAX_PATHSIZ) < 0) {
        syserr(EFAULT);
        return -1;
    }

    if (strnlen(path, MAX_PATHSIZ) == MAX_PATHSIZ) {
        syserr(ENAMETOOLONG);
        return -1;
    }

    ip = namei(path, 0);
    if (ip==NULL) {
        syserr(ENOENT);
        return -1;
    }
    return do_access(ip, mode);
}

int sys_open(struct trap *tf){
    char *path = (char*)tf->ebx;
    int flag = (int)tf->ecx;
    int mode = tf->edx;

    if (vm_verify(path, MAX_PATHSIZ) < 0) {
        syserr(EFAULT);
        return -1;
    }

    if (strnlen(path, MAX_PATHSIZ) == MAX_PATHSIZ) {
        syserr(ENAMETOOLONG);
        return -1;
    }

    return do_open(path, flag, mode);
}

int sys_close(struct trap *tf){
    int fd = tf->ebx;

    return do_close(fd);
}

/* ------------------------------------------- */

/* int read(int fd, char buf[], int cnt); */
int sys_read(struct trap *tf){
    int fd = tf->ebx;
    int cnt = tf->edx;
    char *buf = (char*)tf->ecx;

    if(vm_verify(buf, cnt) < 0){
        syserr(EFAULT);
        return -1;
    }
    return do_read(fd, buf, cnt);
}

/* int write(int fd, char buf[], int cnt); */
int sys_write(struct trap *tf){
    int fd = tf->ebx;
    int cnt = tf->edx;
    char *buf = (char*)tf->ecx;

    if (vm_verify(buf, cnt) < 0) {
        syserr(EFAULT);
        return -1;
    }
    return do_write(fd, buf, cnt);
}

/* int lseek(int fd, unsigned int offset, int whence); */
int sys_lseek(struct trap *tf){
    int fd = (int)tf->ebx;
    uint off = (uint)tf->ecx;
    int whence = (int)tf->edx;

    return do_lseek(fd, off, whence);
}

/* -------------------------------------------- */

int sys_link(struct trap *tf){
    char *path1 = (char*) tf->ebx;
    char *path2 = (char*) tf->ecx;

    return do_link(path1, path2);
}

int sys_unlink(struct trap *tf){
    char *path = (char *) tf->ebx;

    return do_unlink(path);
}

/* int mknod(char *path, int mode, ushort dev) */
int sys_mknod(struct trap *tf){
    char *path = (char*) tf->ebx;
    int mode = (int) tf->ecx;
    int dev = (int) tf->edx;

    return do_mknod(path, mode, dev);
}

/* int creat(char *path, int mode); */
int sys_creat(struct trap *tf){
    char *path = (char*) tf->ebx;
    int mode = (int) tf->ecx;

    return do_creat(path, mode);
}

/* int truncate(char *path, int off) */
int sys_truncate(struct trap *tf){
    syserr(ENOSYS);
    return -1;
}

/* --------------------------------- */

/* int fstat(int fd, struct stat *buf); */
int sys_fstat(struct trap *tf){
    int fd = (int)tf->ebx;
    struct stat *sbuf = (struct stat*)tf->ecx;
    struct file *fp;

    if (fd<0 || fd>=NOFILE || cu->p_ofile[fd]==NULL) {
        syserr(EBADF);
        return -1;
    }
    fp = cu->p_ofile[fd];
    return do_stat(fp->f_ino, sbuf);
}

/* int stat(char *path, struct stat *buf); */
int sys_stat(struct trap *tf){
    char *path = (char*)tf->ebx;
    struct stat *sbuf = (struct stat*)tf->ecx;
    struct inode *ip;

    if (vm_verify(sbuf, sizeof(struct stat))<0) {
        syserr(EFAULT);
        return -1;
    }
    ip = namei(path, 0);
    if (ip==NULL) {
        syserr(ENOENT);
        return -1;
    }
    return do_stat(ip, sbuf);
}

/* int fcntl(int fd, int cmd, int arg); */
int sys_fcntl(struct trap *tf){
    int fd = (int)tf->ebx;
    int cmd = (int)tf->ecx;
    int arg = (int)tf->edx;

    return do_fcntl(fd, cmd, arg);
}

int sys_ioctl(struct trap *tf){
    syserr(ENOSYS);
    return -1;
}

/* --------------------------------- */

/* int dup(int oldfd); */
int sys_dup(struct trap *tf){
    int oldfd = (int)tf->ebx;

    return do_dup(oldfd);
}

/* int dup2(int oldfd, int newfd); */
int sys_dup2(struct trap *tf){
    int oldfd = (int)tf->ebx;
    int newfd = (int)tf->ecx;

    return do_dup2(oldfd, newfd);
}

/* int pipe(int fd[2]); */
int sys_pipe(struct trap *tf){
    return syserr(ENOSYS);
}

/* ----------------------------------------- */

/* int chroot(char *path); */
int sys_chroot(struct trap *tf){
    char *path = (char*)tf->ebx;
    struct inode *ip;

    ip = namei(path, 0);
    if (ip==NULL) {
        syserr(ENOENT);
        return -1;
    }
    if ((ip->i_mode&S_IFMT) != S_IFDIR) {
        syserr(ENOTDIR);
        iput(ip);
        return -1;
    }
    iput(cu->p_iroot);
    cu->p_iroot = ip;
    unlk_ino(ip);
    return 0;
}

/* int chdir(char *path); */
int sys_chdir(struct trap *tf){
    char *path = (char*)tf->ebx;
    struct inode *ip;

    ip = namei(path, 0);
    if (ip==NULL) {
        syserr(ENOENT);
        return -1;
    }
    if ((ip->i_mode&S_IFMT) != S_IFDIR) {
        syserr(ENOTDIR);
        iput(ip);
        return -1;
    }
    iput(cu->p_wdir);
    cu->p_wdir = ip;
    unlk_ino(ip);
    return 0;
}
