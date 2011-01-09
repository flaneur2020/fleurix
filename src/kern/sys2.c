#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>

#include <buf.h>
#include <inode.h>
#include <super.h>
#include <file.h>

/* syscalls on fs */

/* 
 * WARNING: DANGEROUS here! While passing data between kernel
 * and user, a verification is nesscary. Just make a simulation 
 * what the hardware does in user space, or it may cause corruption, 
 * even overlaps the kernel memory, be careful. 
 *
 * TODO: A verify_area() is nessary.
 *
 * */

int sys_access(struct trap *tf){
    char *path = va2pa(tf->ebx);
    int mode = tf->ecx;
    struct inode *ip;

    ip = namei(path, 0);
    if (ip==NULL) {
        syserr(ENOENT);
        return -1;
    }
    return do_access(ip, mode);
}

int sys_open(struct trap *tf){
    char *path = va2pa(tf->ebx);
    int flag = tf->ecx;
    int mode = tf->edx;

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
    char *buf = va2pa(tf->ecx); 
    int cnt = tf->edx;

    return do_read(fd, buf, cnt);
}

/* int write(int fd, char buf[], int cnt); */
int sys_write(struct trap *tf){
    int fd = tf->ebx;
    char *buf = va2pa(tf->ecx);
    int cnt = tf->edx;

    return do_write(fd, buf, cnt);
}

/* int lseek(int fd, unsigned int offset, int whence); */
int sys_lseek(struct trap *tf){
}

/* -------------------------------------------- */

int sys_link(struct trap *tf){
}

int sys_unlink(struct trap *tf){
}

int sys_mknod(struct trap *tf){
}

int sys_creat(struct trap *tf){
}

/* --------------------------------- */

int sys_stat(struct trap *tf){
}

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
