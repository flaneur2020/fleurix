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

void sys_access(struct trap *tf){
    char *path = va2pa(tf->ebx);
    int mode = tf->ecx;

    printf("access(); %x\n", path);
    //printf("path: %x\n", path);
}

void sys_open(struct trap *tf){
}

void sys_close(struct trap *tf){
}

void sys_read(struct trap *tf){
}

void sys_write(struct trap *tf){
}

void sys_fstat(struct trap *tf){
}

void sys_link(struct trap *tf){
}

void sys_unlink(struct trap *tf){
}

void sys_mkdir(struct trap *tf){
}

void sys_mknod(struct trap *tf){
}
