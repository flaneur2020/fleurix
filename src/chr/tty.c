#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <tty.h>
#include <keybd.h>

struct tty tty0;

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
        case CERASE:
            eraseq(&tp->t_canq);
            break;
        default:
            putq(&tp->t_canq, ch);
        }
    }
}

/* output characters with buffering */
int tty_output(struct tty *tp, char ch){
    putq(&tp->t_outq, ch);
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

    if (tp->t_flag & TTY_RAW){
        // TODO
    }
    if (ch==CEOF || ch=='\n') {
        eraseq(&tp->t_canq);
        wakeup(tp);
        return 0;
    }
}

/* ---------------------------------------- */

int tty_start(struct tty *tp){
    int (*putc)(char);
    char ch;

    putc = tp->t_putc;
    while((ch=getq(&tp->t_outq)) >= 0){
        putc(ch);
    }
}

/* ---------------------------------------------- */

/*
 * If the list is not full, wait until
 * */
int tty_read(struct tty *tp, char *buf, uint cnt){
    struct qbuf qb;
    char ch;
    int i;

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

int tty_write(struct tty *tp, char *buf, uint cnt){
}

/* ---------------------------------------------- */

void tty_init(){
    tty0.t_flag = TTY_ECHO;
    tty0.t_putc = &putch;
    tty0.t_rawq.q_count = 0;
    tty0.t_canq.q_count = 0;
    tty0.t_outq.q_count = 0;
}
