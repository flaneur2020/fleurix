/*
 * hdrw.c
 * This file implies the driver for hard disk.
 * */

#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>
#include <buf.h>

struct devtab hdtab = { 0 , };

int hdreq(){
}

int hdrw() {
}

int hdcmd(int c, int h, int s, int ns, uint addr) {
}

int hdinit(){
}

/******************************************************/

/* empty routine */
int nulldev(){
}

/* it should raise an ENODEV error when being called.*/
int nodev(){
}
