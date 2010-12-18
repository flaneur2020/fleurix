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
#define O_ACCMODE	00003
#define O_RDONLY	   00
#define O_WRONLY	   01
#define O_RDWR		   02
#define O_CREAT		00100	/* not fcntl */
#define O_EXCL		00200	/* not fcntl */
#define O_NOCTTY	00400	/* not fcntl */
#define O_TRUNC		01000	/* not fcntl */
#define O_APPEND	02000
#define O_NONBLOCK	04000	/* not fcntl */

#endif
