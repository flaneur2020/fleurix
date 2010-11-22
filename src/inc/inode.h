
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

/* constants helper */
#define NINO_PER_BLK (BLK/(sizeof(struct d_inode)))
