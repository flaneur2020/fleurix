#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
//
#include <conf.h>
#include <tty.h>
#include <keybd.h>

struct tty tty[NTTY];

/* ---------------------------------------------- */

/*
 * Append a character into a tty buf.
 * */
int putq(struct qbuf *qb, char ch){
    // this buffer has been full now.
    if (qb->q_count == QBUFSIZ) {
        return -1;
    }
    qb->q_char[qb->q_end] = ch;
    qb->q_end = (qb->q_end + 1) % QBUFSIZ;
    qb->q_count++;
    return 0;
}

/* Get the first character from a tty buf. 
 * */
char getq(struct qbuf *qb){
    char ch;
    // if this buffer is empty
    if (qb->q_count == 0) {
        return -1;
    }
    ch = qb->q_char[qb->q_start];
    qb->q_start = (qb->q_start + 1) % QBUFSIZ;
    qb->q_count--;
    return ch;
}

/* Erase the last character of a tty buf */
char eraseq(struct qbuf *qb){
    // if this buffer is empty
    if (qb->q_count == 0) {
        return -1;
    }
    qb->q_end = (qb->q_end-1 + QBUFSIZ) % QBUFSIZ;
    qb->q_count--;
    return 0;
}

/* ---------------------------------------------- */

/* take characterers from raw list, make some parse & erase and place
 * it into canon list. */
int tty_canon(struct tty *tp){
    char ch;
    int i;
    
    // if raw mode
    if (tp->t_flag & TTY_RAW) {
        while ((ch=getq(&tp->t_rawq)) >= 0) {
            putq(&tp->t_canq, ch);
        }
        return 0;
    }
    // canon mode
    while ((ch=getq(&tp->t_rawq)) >= 0) {
        switch(ch){
        case '\t':
            // expand tab into spaces
            for(i=0; i<4-(tp->t_col%4); i++)
                putq(&tp->t_canq, ' ');
            return 0;
            break;
        case CERASE:
            eraseq(&tp->t_canq);
            break;
        default:
            putq(&tp->t_canq, ch);
            break;
        }
    }
}

/* output characters with buffering */
int tty_output(struct tty *tp, char ch){
    int i;
    // 
    switch(ch){
    case '\t':
        // expand tab into spaces
        for(i=0; i<4-(tp->t_col%4); i++) 
            putq(&tp->t_outq, ' ');
        break;
    default:
        putq(&tp->t_outq, ch);
        break;
    }
    return 0;
}

/*
 * Place a character on raw TTY input queue, if a carriage character 
 * arrives, wake up the awaitors.
 * */
int tty_input(struct tty *tp, char ch){
    uint cnt;

    putq(&tp->t_rawq, ch);
    cnt = tp->t_canq.q_count;
    tty_canon(tp);

    if (tp->t_flag & TTY_ECHO) {
        if (ch == CERASE) {
            if (cnt <= 0) {
                return 0;
            }
        }
        tty_output(tp, ch);
        tty_start(tp);
    }

    if ((tp->t_flag & TTY_RAW)==0){
        if (CINTR==ch) {
            sigsend_g(tp->t_pgrp, SIGINT, 0);
        }
    }
    if (ch==CEOF || ch=='\n') {
        eraseq(&tp->t_canq);
        wakeup(tp);
        return 0;
    }
}

/* ---------------------------------------- */

/* output actually starts here. */
int tty_start(struct tty *tp){
    int (*putc)(char);
    char ch;

    putc = tp->t_putc;
    while((ch=getq(&tp->t_outq)) >= 0){
        putc(ch);
    }
}

/* ---------------------------------------------- */

int tty_open(ushort dev){
    struct tty *tp;

    if (MINOR(dev) >= NTTY){
        syserr(ENODEV);
        return -1;
    }
    tp = &tty[MINOR(dev)];
    tp->t_flag = TTY_ECHO;
    tp->t_putc = &putch;
    tp->t_rawq.q_count = 0;
    tp->t_canq.q_count = 0;
    tp->t_outq.q_count = 0;
    tp->t_pgrp = cu->p_pgrp;
    return 0;
}

/* called in iput() */
int tty_close(ushort dev){
    return 0;
}

/* ---------------------------------------------- */

/*
 * If the list is not full, wait until
 * */
int tty_read(ushort dev, char *buf, uint cnt){
    struct tty *tp;
    struct qbuf qb;
    char ch;
    int i;

    if (MINOR(dev) >= NTTY){
        syserr(ENODEV);
        return -1;
    }
    tp = &tty[MINOR(dev)];
    // if no data on canonical list
    if (tp->t_canq.q_count < cnt) {
        sleep(tp, PRITTY);
    }
    // 
    for (i=0; i<cnt; i++) {
        if ((ch=getq(&tp->t_canq)) < 0)
            break;
        buf[i] = ch;
    }
    return i+1;
}

int tty_write(ushort dev, char *buf, uint cnt){
    struct tty *tp;
    int i;
    
    if (MINOR(dev) >= NTTY){
        syserr(ENODEV);
        return -1;
    }
    tp = &tty[MINOR(dev)];
    for (i=0; i<cnt; i++) {
        tty_output(tp, buf[i]);
    }
    tty_start(tp);
    return 0;
}

/* ---------------------------------------------- */

void tty_init(){
    tty[0].t_flag = TTY_ECHO;
    tty[0].t_putc = &putch;
    tty[0].t_rawq.q_count = 0;
    tty[0].t_canq.q_count = 0;
    tty[0].t_outq.q_count = 0;
}
