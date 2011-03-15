/* */

#ifndef CONF_H
#define CONF_H

struct bdevsw {
    int             (*d_open)(); 
    int             (*d_close)();
    int             (*d_request)(struct buf *bp);
    struct devtab    *d_tab;
};

extern struct bdevsw    bdevsw[NBLKDEV];   

struct cdevsw {
    int             (*d_open)   (ushort dev);
    int             (*d_close)  (ushort dev);
    int             (*d_read)   (ushort dev, char *buf, uint cnt);
    int             (*d_write)  (ushort dev, char *buf, uint cnt);
    int             (*d_sgtty)();
};

extern struct cdevsw    cdevsw[NCHRDEV];

#define MAJOR(num) ((uchar)((num>>8)&0xFF))
#define MINOR(num) ((uchar)((num)&0xFF))

/* dev number is an ushort */
#define DEVNO(major, minor) ((ushort)(((uchar)major<<8) + (uchar)minor))

#define NODEV (DEVNO(0,0))

extern ushort rootdev;
extern ushort swapdev;

#endif
