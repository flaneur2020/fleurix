#ifndef TTY_H
#define TTY_H

#define NCCS 17
#define TTY_BUFSIZ 1024

struct clist {
    char    *c_head;
    char    *c_tail;
    char     c_buf[TTY_BUFSIZ];
};

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

