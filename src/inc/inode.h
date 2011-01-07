#ifndef INODE_H
#define INODE_H

/* 
 * the Minix v1 fs, you could build one via mkfs.minix under linux.
 * */

struct d_inode {
	ushort  i_mode;     /* file type, protection, etc. */
	ushort  i_uid;
	uint    i_size;     /* current file size in bytes */
	uint    i_time;
	uchar   i_gid;
	uchar   i_nlinks;   /* how many links to this file. */
	ushort  i_zone[9];  /* zones number for direct, indirect and double indirect. */
};

struct inode {
	ushort  i_mode;
	ushort  i_uid;
	uint    i_size;
	uint    i_time;
	uchar   i_gid;
	uchar   i_nlinks;
	ushort  i_zone[9];
    /* the rest only lies in core */
    ushort  i_count;    /* reference count*/
    ushort  i_dev;      /* dev number */
    uint    i_num;      /* inode number */
    uint    i_flag;
};

extern struct inode inode[NINODE];

/* flags */
#define I_LOCK   0x1    /* for synchronize */
#define I_WANTED 0x2    /* - */
#define I_DIRTY  0x4    /* this inode has been modified*/
#define I_MOUNT  0x8    /* - */
#define I_TEXT   0x10   /* this inode is a running executable */

/* i_mode resides in stat.h */

/* directory entry */
#define NAMELEN 12

struct dirent {
    ushort  d_ino;
    char    d_name[NAMELEN];
    char    __p[18]; /* a padding. each dirent is aligned with a 32 bytes boundary. */
};

#endif
