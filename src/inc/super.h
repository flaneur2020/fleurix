/* it's a minix 3 fs's super block.
 * note that each minor device's first logical block(1024b, 2 physical block)
 * is reserved for the boot record, the second logical block is reserved for the 
 * super block, which is also 1024b.
 * */
struct super {
    ushort  s_max_inode;
    ushort  s_max_zone;
    ushort  s_nimap_blk;
    ushort  s_nzmap_blk;
    ushort  s_zone0;
    ushort  s_log_zone;
    uint    s_max_size;
    ushort  s_magic;
    ushort  s_nzone;
    /* the rest fields lie in-core */
};

extern struct super     super[NSUPER];
