#ifndef TTY_H
#define TTY_H

/* down blow might be confusing, but they've got no account with each other. */
#define CBSIZE   500 // size of a cblock. 
#define CANBSIZ  512 // size of a canonical buffer, the limit of each line input.

//
struct cblock {
    struct cblock  *cb_next;
    struct cblock  *cb_prev;
    uint            cb_start;
    uint            cb_end;
    char            cb_char[CBSIZE];
};

/*
 * Each clist is a queue of struct cblock, and each cblock
 * has a fixed-size array of characters with two offsets
 * which indicates the range from cb_start to cb_end.
 * */
struct clist {
    uint            c_count; /* the total characters count this stored. */
    struct cblock  *c_first; 
    struct cblock  *c_last;
};

/*
 * */
struct tty {
    int     t_flag;
    int     t_dev;
    struct clist    t_rawq;
    struct clist    t_canq;
    struct clist    t_outq;
};

extern struct tty tty0;

/* control characters */
#define CINTR   003  /* ctrl-C */
#define CQUIT   034  /* ctrl-\ */
#define CERASE  0177 /* DEL, BS */
#define CKILL   025  /* ctrl-X */
#define CEOF    004  /* ctrl-D */
#define CEOL    00
#define CSTART  021 /* ctrl-Q */
#define CSTOP   023 /* ctrl-S */
#define CSUSP   032 /* ctrl-Z */

#endif

