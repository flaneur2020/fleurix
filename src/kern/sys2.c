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

int sys_read(struct trap *tf){
    int fd = tf->ebx;
    char *buf = va2pa(tf->ecx);
    int cnt = tf->edx;

    return do_read(fd, buf, cnt);
}

int sys_write(struct trap *tf){
    int fd = tf->ebx;
    char *buf = va2pa(tf->ecx);
    int cnt = tf->edx;

    return do_write(fd, buf, cnt);
}

int sys_link(struct trap *tf){
}

int sys_unlink(struct trap *tf){
}

int sys_fstat(struct trap *tf){
}

int sys_mkdir(struct trap *tf){
}

int sys_mknod(struct trap *tf){
}
