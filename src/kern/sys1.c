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
    char *path;
    int mode;

    path = tf->eax;
    mode = tf->ebx;
}

int sys_open(struct trap *tf){
}

int sys_close(struct trap *tf){
}

int sys_read(struct trap *tf){
}

int sys_write(struct trap *tf){
}

int sys_fstat(struct trap *tf){
}

int sys_link(struct trap *tf){
}

int sys_unlink(struct trap *tf){
}

int sys_mkdir(struct trap *tf){
}

int sys_mknod(struct trap *tf){
}
