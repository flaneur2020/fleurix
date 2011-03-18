#ifndef DIRENT_H
#define DIRENT_H

/* directory entry */
#define NAMELEN 12

struct dirent {
    ushort  d_ino;
    char    d_name[NAMELEN];
    char    __p[18]; /* a padding. each dirent is aligned with a 32 bytes boundary. */
};

#endif
