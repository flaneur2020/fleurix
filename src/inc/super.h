/* it's a minix 2 fs's super block.
 * note that each minor device's first logical block(1024b, 2 physical block)
 * is reserved for the boot record, the second logical block is reserved for the 
 * super block, which is also 1024b.
 * */
struct d_super {
    ushort  s_max_inode;    // -
    ushort  s_max_zone;     // -
    ushort  s_nimap_blk;    // number of inode bit map blocks
    ushort  s_nzmap_blk;    // number of zone bitmap blocks
    ushort  s_zone0;        // number of the first data zone.
    ushort  s_log_bz;       // log(blk,zone), lba == nr_zone >> s_log_bz
    uint    s_max_size;     // the max size of one file (in bytes)
    ushort  s_magic;        // 0x138F
    ushort  s_nzone;        // number of zones in used.
};

struct super {
    ushort  s_max_inode;    // -
    ushort  s_max_zone;     // -
    ushort  s_nimap_blk;    // number of inode bit map blocks
    ushort  s_nzmap_blk;    // number of zone bitmap blocks
    ushort  s_zone0;        // number of the first data zone.
    ushort  s_log_bz;       // log(blk,zone), lba == nr_zone >> s_log_bz
    uint    s_max_size;     // the max size of one file (in bytes)
    ushort  s_magic;        // 0x138F
    ushort  s_nzone;        // number of zones in used.
    /* the rest fields only lie in-core, for mount */
    short           s_dev;
    struct inode   *s_iroot; // the root inode of this super block
    struct inode   *s_imnt;  // the mount point, like /mnt/dir
    uint            s_flag;
};

extern struct super     mnt[NMOUNT];

#define S_MAGIC 0x138F

/* flags */
#define S_LOCK      0x1
#define S_WANTED    0x2
#define S_RDONLY    0x4
#define S_DIRTY     0x8

