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
    /* the rest fields only lie in-core */
    short           s_dev;
    struct inode   *s_iroot;
    struct inode   *s_imnt;
    uchar           s_lock;
    uchar           s_rd_only;
    uchar           s_dirty;
};


#define SB_MAGIC 0x138F
