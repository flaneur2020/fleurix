/**/

struct mount {
    short           m_dev;
    struct super    m_super;
};

extern struct mount mnt[NMOUNT];
