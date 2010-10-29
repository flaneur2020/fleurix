#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>

#include <buf.h>
#include <conf.h>
#include <hdrw.h>

/*
 * hdrw.c
 * This file implies the driver for hard disk.
 * */

struct devtab hdtab = { 0 , };

/*
 * Send a request for the hard disk drive.
 * */
int hd_req(struct buf *bp){
}

int do_hd_intr(){
}

int hd_cmd(uint drv, uint lba, uint cmd) {
}

void hd_wait_ready(){
}

void hd_init(){
    // Allow the hard disk controller invoking interrupts.
    outb(0x21, inb(0x21) & 0xFB);
    outb(0xA1, inb(0xA1) & 0xBF);
}

/******************************************************/

/* empty routine */
int nulldev(){
}

/* it should raise an ENODEV error when being called.*/
int nodev(){
}

