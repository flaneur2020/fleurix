
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
};
