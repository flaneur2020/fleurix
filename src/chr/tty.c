#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <tty.h>
#include <keybd.h>

struct tty tty0;

/* ---------------------------------------------- */

int cputc(struct clist *cp, char ch){
}

char cgetc(struct clist *cp){
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
