#ifndef FILE_H
#define FILE_H

#include <inode.h>

struct file {
    char            f_flag;
    char            f_count;
    uint            f_offset;
    struct inode   *f_ino;
};

extern struct file file[NFILE];

/* fcntl */
#define O_ACCMODE	0x3

#define O_RDONLY	0x0     // read only
#define O_WRONLY	0x1     // write only
#define O_RDWR		0x2     // read / write
#define O_APPEND	0x40    // 
/* the rest can not be modified by fcntl */
#define O_CREAT		0x4     // creat a new file.
#define O_EXCL		0x8     // a execulsive open 
#define O_NOCTTY	0x10    // 
#define O_TRUNC		0x20    //
#define O_NONBLOCK	0x80    //

#endif
