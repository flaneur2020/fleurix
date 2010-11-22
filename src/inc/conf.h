/* */

struct bdevsw {
    int             (*d_open)(); 
    int             (*d_close)();
    int             (*d_request)();
    struct devtab    *d_tab;
};

extern struct bdevsw    bdevsw[NBLKDEV];   

struct cdevsw {
    int             (*d_open)();
    int             (*d_close)();
    int             (*d_read)();
    int             (*d_write)();
    int             (*d_sgtty)();
};

extern struct cdevsw    cdevsw[];

#define MAJOR(num) ((uchar)((num>>8)&0xFF))
#define MINOR(num) ((uchar)((num)&0xFF))

#define DEVNO(major, minor) ((ushort)(((uchar)major<<8) + (uchar)minor))

#define NODEV (DEVNO(0,0))
