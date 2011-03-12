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

int sys_access(struct trap *tf){
    char *path = tf->ebx;
    int mode = tf->ecx;
    struct inode *ip;
    int r;

    r = vm_verify(path, strlen(path)+1);

    ip = namei(path, 0);
    if (ip==NULL) {
        syserr(ENOENT);
        return -1;
    }
    return do_access(ip, mode);
}

int sys_open(struct trap *tf){
    char *path = tf->ebx;
    int flag = tf->ecx;
    int mode = tf->edx;
    int r;

    r = vm_verify(path, strlen(path)+1);

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
    char *buf = tf->ecx; 
    int r;

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
    char *buf = tf->ecx;
    int r;

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

int sys_creat(struct trap *tf){
    char *path = (char*) tf->ebx;
    int mode = (int) tf->ecx;

    return do_creat(path, mode);
}

/* --------------------------------- */

/* int stat(const char *path, struct stat *buf); */
int sys_stat(struct trap *tf){
    char *path = (char*) tf->ebx;
    struct stat *buf = (struct stat*) tf->ecx;
    struct inode *ip;

    ip = namei(path, 0);
    if (ip==NULL){
        return -1;
    }
    if (vm_verify(buf, sizeof(struct stat)) < 0) {
        syserr(EFAULT);
        return -1;
    }
    return do_stat(ip, buf);
}

/* int stat(const char *path, struct stat *buf); */
int sys_fstat(struct trap *tf){
}

int sys_fcntl(struct trap *tf){
}

int sys_ioctl(struct trap *tf){
}

/* --------------------------------- */

/* int dup(int oldfd); */
int sys_dup(struct trap *tf){
}

/* int dup2(int oldfd, int newfd); */
int sys_dup2(struct trap *tf){
}

/* int pipe(int fd[2]); */
int sys_pipe(struct trap *tf){
}

