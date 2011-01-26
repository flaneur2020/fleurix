#ifndef TTY_H
#define TTY_H

struct cblock {
    char            c_body[60]; 
    struct cblock  *c_next;
};

struct clist {
    uint            c_num;
    struct cblock  *c_first;
};


struct tty {
    int     t_flag;
    int     t_dev;
    struct clist    t_rawq;
    struct clist    t_canq;
    struct clist    t_outq;
};

#endif
