#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <tty.h>
#include <keybd.h>

struct tty tty0;

/* only used in tty_canon() */
char canonb[CANBSIZ];

/* ---------------------------------------------- */

/*
 * Append a character into clist.
 * */
int cl_putc(struct clist *cl, char ch){
    struct cblock *cb;

    // if this clist is null
    if (cl->c_last == NULL) {
        cb = (struct cblock*)kmalloc(sizeof(struct cblock));
        cb->cb_next = NULL;
        cb->cb_start = 0;
        cb->cb_end = 0;
        //
        cl->c_first = cb;
        cl->c_last = cb;
        cl->c_count = 0;
    }
    // find a availible cblock
    cb=cl->c_last;
_find:
    for (; cb; cb=cb->cb_next) {
        if (cb->cb_end < CBSIZE-1) {
            cb->cb_char[cb->cb_end] = ch;
            cb->cb_end++;
            //
            cl->c_count++;
            return 0;
        }
    }
    // no free cblock, allocate and append one
    cb = (struct cblock*)kmalloc(sizeof(struct cblock));
    cb->cb_next = NULL;
    cb->cb_start = 0;
    cb->cb_end = 0;
    //
    cl->c_last->cb_next = cb;
    cl->c_last = cb;
    goto _find;
}

/* Get the first character from a clist. 
 * cblocks only freed here.
 * */
char cl_getc(struct clist *cl){
    struct cblock *cb;
    char ch;

    if (cl->c_first==NULL) {
        return -1;
    }

_pop:
    cb = cl->c_first;
    if (cb==NULL) 
        return -1;
    if (cb->cb_start == cb->cb_end) {
        cl->c_first = cb->cb_next;
        if (cb == cl->c_last) {
            cl->c_last = NULL;
        }
        kfree(cb, sizeof(struct cblock));
        goto _pop;
    }

    ch = cb->cb_char[cb->cb_start];
    cb->cb_start++;
    return ch;
}

/* ---------------------------------------------- */

/*
 * Place a character on raw TTY input queue, do
 * */
int tty_input(struct tty *tp, char ch){
    putch(ch);
}

int tty_canon(struct tty *tp){
}

int tty_output(struct tty *tp, char ch){
}

/* ---------------------------------------------- */

int ttread(struct tty *tp){
}

int ttwrite(struct tty *tp){
}

/* ---------------------------------------------- */

void tty_init(){
}
