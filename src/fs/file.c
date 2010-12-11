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

// 
struct file* falloc();
int ufalloc();

/*
 * 
 * */
int do_open(char *path, uint mode){
    struct inode *ip;
    struct file *fp;
    int fd;

    // TODO: on create a new file.
    if (mode & O_CREATE){
    }
    // an existing file.
    else {
        ip=do_namei(path);
        if (ip == NULL){
            iput(ip);
            current->p_error = ENFILE;
            return -1;
        }
        // TODO: check access and special files
    } 

    if ((fd=ufalloc()<0) || (fp=falloc(fd))==NULL) {
        unlock_inode(ip);
        return -1;
    }
    unlock_inode(ip);
    fp->f_flag = mode;
    fp->f_inode = ip;
    return fd;
}

int do_close(int fd){
}


/* -------------------------------------------------------------- */

int do_read(int fd, char *buf, int count){
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
