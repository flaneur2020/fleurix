
struct d_inode {
	ushort  i_mode;
	ushort  i_uid;
	uint    i_size;
	uint    i_time;
	uchar   i_gid;
	uchar   i_nlinks;
	ushort  i_zone[9];
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
    uint    i_num;      /* */
    uint    i_flag;
};

/* flags */
#define I_LOCK   0x1    /* for synchronize */
#define I_WANTED 0x2    /* - */
#define I_DIRTY  0x4    /* this inode has been modified*/
#define I_MOUNT  0x8    /* - */

/* formats */
#define S_IFMT  00170000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

/* constants helper */
#define NINO_PER_BLK (BLK/(sizeof(struct d_inode)))
