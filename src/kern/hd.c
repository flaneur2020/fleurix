#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>

#include <buf.h>
#include <conf.h>
#include <hd.h>

/*
 * hd.c - driver for the hard disk.
 * 
 * Many constants, refered from http://wiki.osdev.org/IDE.
 * Thanks buddy.
 * */

#define IDE_CMD_READ              0x20
#define IDE_CMD_WRITE             0x30

#define IDE_BSY	    0x80 
#define IDE_DRDY	0x40 //Device Ready
#define IDE_DF		0x20 //Device Fault
#define IDE_ERR		0x01

struct devtab hdtab = { 0 , };

/* 
 * Drive number can only be 0 or 1.
 * lba is 28-bit.
 * Number of sectors must <= 255.
 *
 * note: 0x1F6 is the register HDDEVSEL, which (might) stands for HD DEV SELECTOR.
 * if bit 6 of this register is set, we are going to use LBA as addressing mode.
 * In LBA mode, the (8 bits) register 0x1F3,0x1f4,0x1F5 and the lower 4 bits of 0x1F6, 
 * forms a 28-bit LBA address, so called LBA-28.  
 * */
int hd_cmd(uint drive, uint cmd, uint lba, uchar ns) {
    if (!hd_wait_ready()){
        //panic("hd_cmd: hd controller not ready. ");
    }
    outb(0x3F6, 0);  // generate interrupt
    outb(0x1F2, ns);  // number of sectors
    outb(0x1F3, lba & 0xFF);
    outb(0x1F4, (lba >> 8) & 0xFF);
    outb(0x1F5, (lba >> 16) & 0xFF);
    outb(0x1F6, 0xE0 | ((drive&1)<<4) | ((lba>>24)&0x0F));
    outb(0x1F7, cmd);
}

/* Wait til controller Ready or timeout. */
int hd_wait_ready(){
    int nr = 10000, r;
    while(--nr && !(inb(0x1F7) & IDE_DRDY)){
        /* do nothing */;
    }
    return nr;
}

/* Send a request for the hard disk drive. */
int hd_req(struct buf *bp){
    if (!hd_wait_ready()){
        //panic("hd_req: hd controller not ready.");
    }
    // read or write.
    if (bp->b_flag & B_READ) {
        hd_cmd(0, IDE_CMD_READ, bp->b_blkno, 1);
    }
    else {
        hd_cmd(0, IDE_CMD_WRITE, bp->b_blkno, 1);
        outsl(0x1F0, bp->b_addr, 512/4);
    }
}

int do_hd_intr(struct trap_frame *tf){
    puts("do_hd_intr: got data\n");
}

/*
 * 
 * */
void hd_init(){
    // Allow the hard disk controller invoking interrupts.
    outb(0x21, inb(0x21) & 0xFB);
    outb(0xA1, inb(0xA1) & 0xBF);
    // Set the interrupt handler.
    set_hwint(0x2E, &do_hd_intr);
}

/******************************************************/

/* empty routine */
int nulldev(){
}

/* it should raise an ENODEV error when being called.*/
int nodev(){
}

