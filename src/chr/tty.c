#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <tty.h>
#include <keybd.h>

int cputc(struct clist *cp, char ch){
}

char cgetc(struct clist *cp){
}

/* ---------------------------------------------- */

int ttread(struct tty *tp){
}

int ttcanon(struct tty *tp){
}

int ttwrite(struct tty *tp){
}
