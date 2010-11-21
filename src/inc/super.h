/* it's a minix 3 fs's super block.
 * note that each minor device's first logical block(1024b, 2 physical block)
 * is reserved for the boot record, the second logical block is reserved for the 
 * super block, which is also 1024b.
 * */
struct super {
    uint    s_max_ninode;
    ushort  s_max_nzone;
    ushort  s_nimap_blk;
    ushort  s_nzmap_blk;
    ushort  s_zone0;
    ushort  s_log_zone;
    ushort  __p1;
    uint    s_max_size;
    uint    s_nzone;
    short   s_magic;
    short   __p2;
    ushort  s_blk_size;
    char    s_fs_version;
    /* the rest fields lie in-core */
};
