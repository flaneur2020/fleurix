/* */

struct bdevsw {
    int             (*d_open)(); 
    int             (*d_close)();
    int             (*d_request)();
    struct devtab    *d_tab;
};

extern struct bdevsw    bdevsw[];   

extern int nblkdev;

struct cdevsw {
    int             (*d_open)();
    int             (*d_close)();
    int             (*d_read)();
    int             (*d_write)();
    int             (*d_sgtty)();
};

extern struct cdevsw    cdevsw[];

#define MAJOR(num) ((char)((num>>8)&0xFF))
#define MINOR(num) ((char)((num)&0xFF))

#define DEVNO(major, minor) ((short)(((char)major<<8) + (char)minor))
