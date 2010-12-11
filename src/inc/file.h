#ifndef FILE_H
#define FILE_H

#include <inode.h>

struct file {
    char            f_flag;
    char            f_count;
    uint            f_offset;
    struct inode   *f_inode;
};

extern struct file file[NFILE];

#define F_READ  0x1
#define F_WRITE 0x2

/* fcntl */
#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200

#endif
