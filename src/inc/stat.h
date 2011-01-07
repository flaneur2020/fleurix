#ifndef STAT_H
#define STAT_H

struct stat {
    ushort  st_dev; 
    ushort  st_ino;
    ushort  st_mode;
    ushort  st_nlink;  
    ushort  st_uid;
    ushort  st_gid;
    ushort  st_rdev;
    uint    st_size;  
    ushort  st_atime;
    ushort  st_mtime;
    ushort  st_ctime;
};

#define S_IFMT  00170000
#define S_IFREG  0100000 /* regular */
#define S_IFBLK  0060000 /* blk device */
#define S_IFDIR  0040000 /* directory */
#define S_IFCHR  0020000 /* char device*/
#define S_IFIFO  0010000 /* fifo */
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

/* access */
#define RWX_MODES 0000777	/* mode bits for RWX only */
#define R_BIT    0000004	/* Rwx protection bit */
#define W_BIT    0000002	/* rWx protection bit */
#define X_BIT    0000001	/* rwX protection bit */

#endif
