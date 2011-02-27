#ifndef FILE_H
#define FILE_H

#include <inode.h>

struct file {
    char            f_oflag;
    char            f_fdflag; // only one FD_CLOEXEC by default.
    char            f_count;
    uint            f_offset;
    struct inode   *f_ino;
};

extern struct file file[NFILE];

/* flags in open/fcntl */
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

/* fcntl */
#define F_DUPFD		0	/* dup */
#define F_GETFD		1	/* seems only FD_CLOEXEC associated with this. */ 
#define F_SETFD		2	
#define F_GETFL		3	/* get f_flag */
#define F_SETFL		4   /* set f_flag */
#define F_GETLK		5	/* aquire a lock. (not implemented) */
#define F_SETLK		6   
#define F_SETLKW	7

#define FD_CLOEXEC 1    /* close this file on exec. (not implemented) */

/* seek */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* access */
#define F_OK	0
#define X_OK	1
#define W_OK	2
#define R_OK	4

#endif
