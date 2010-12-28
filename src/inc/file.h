#ifndef FILE_H
#define FILE_H

#include <inode.h>

struct file {
    char            f_mode;
    char            f_flag;
    char            f_count;
    uint            f_offset;
    struct inode   *f_ino;
};

extern struct file file[NFILE];

/* fcntl */
#define O_ACCMODE	0x3
#define O_RDONLY	0x0
#define O_WRONLY	0x1
#define O_RDWR		0x2
#define O_CREAT		0x4	    /* not fcntl */
#define O_EXCL		0x8	    /* not fcntl */
#define O_NOCTTY	0x10	/* not fcntl */
#define O_TRUNC		0x20	/* not fcntl */
#define O_APPEND	0x40
#define O_NONBLOCK	0x80	/* not fcntl */

#endif
