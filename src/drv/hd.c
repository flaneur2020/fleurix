#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <buf.h>
#include <conf.h>
#include <hd.h>

/*
 * hd.c - driver for the hard disk.
 * Ingore the second ide drive right now.
 * 
 * Many constants, refered from http://wiki.osdev.org/IDE.
 * Thanks buddy.
 * */

struct devtab hdtab = { 0 , };

/* 
 * Drive number can only be 0 or 1.
 * lba is 28-bit.
 * Number of sectors must <= 255.
 *
 * note: 0x1F6 is the register HD_DEVSEL, which (might) stands for HD DEV SELECTOR.
 * if bit 6 of this register is set, we are going to use LBA as addressing mode.
 * In LBA mode, the (8 bits) register 0x1F3,0x1f4,0x1F5 and the lower 4 bits of 0x1F6, 
 * forms a 28-bit LBA address, so called LBA-28.  
 * */
int hd_cmd(uint drive, uint cmd, uint lba, uchar ns) {
    hd_wait_ready();
    outb(0x3F6, 0);  // generate interrupt
    outb(0x1F2, ns);  // number of sectors
    outb(0x1F3, lba & 0xFF);
    outb(0x1F4, (lba >> 8) & 0xFF);
    outb(0x1F5, (lba >> 16) & 0xFF);
    outb(0x1F6, 0xE0 | ((drive&1)<<4) | ((lba>>24)&0x0F));
    outb(HD_CMD, cmd);
}

/* 
 * When we send a command, we should wait for 400 nanosecond, then read the Status 
 * port. If the Busy bit is on, we should read the status port again until the Busy bit is 0; 
 * then we can read the results of the command. This operation is called "Polling". We can also 
 * use IRQs instead of polling.
 * */
int hd_wait_ready(){
    int i = 10000, r;
    while(--i || ((r=inb(HD_STAT)) & HD_BSY)){
        /* do nothing */;
    }
    return i;
}

/* prepend it into devtab's waiting list(via av_prev, av_next),
 * if it's not active, send a request for the hard disk drive right
 * now. 
 * */
int hd_request(struct buf *bp){
    // prepend into the waiting list 
    bp->av_prev = &hdtab;
    bp->av_next = hdtab.av_next;
    hdtab.av_next->av_prev = bp;
    hdtab.av_next = bp;
    // if the device's not busy
    if (hdtab.d_active == 0) {
        hd_start();
    }
}

/* if waiting list is not empty, then take the tail, send a request 
 * for the hard disk drive and mark it active.
 * note: BSIZE = (sizeof logical block) / (sizeof disk block), so 
 * physical address = lba * BSIZE
 * TODO: don't support two hd disk yet.
 * */
void hd_start(){
    struct buf *bp;
    // if waiting list is empty
    if (hdtab.av_next == &hdtab) {
        return;
    }
    // take the tail
    bp = hdtab.av_prev; 
    hdtab.d_active = 1;
    // read or write.
    if (bp->b_flag & B_READ) {
        hd_cmd(0, HD_CMD_READ, bp->b_blkno*BSIZE/PBSIZE, BSIZE/PBSIZE);
    }
    else {
        hd_cmd(0, HD_CMD_WRITE, bp->b_blkno*BSIZE/PBSIZE, BSIZE/PBSIZE);
        outsl(0x1F0, bp->b_data, BSIZE/4);
    }
}

/* interrupt handler of the hard disk drive, triggered on got 
 * data from the hard disk. get and remove the tail from the 
 * waiting list, if there's still something inside it, hd_start()
 * again.
 * */
int do_hd_intr(struct trap *tf){
    struct buf *bp;

    if (hdtab.d_active == 0) {
        return;
    } 
    hdtab.d_active = 0;
    bp = hdtab.av_prev;
    bp->av_prev->av_next = bp->av_next;
    bp->av_next->av_prev = bp->av_prev;
    // read data if needed
    if (bp->b_flag & B_READ) {
        insl(0x1F0, bp->b_data, BSIZE/4);
    }
    iodone(bp);
    hd_start();
}

/*
 * Allow the hard disk controller invoking interrupts
 * and set the interrupt handler.
 * */
void hd_init(){
    irq_enable(14);
    set_hwint(IRQ0+14, &do_hd_intr);
}

/******************************************************/

/* empty routine */
int nulldev(){
}

/* it should raise an ENODEV error when being called.*/
int nodev(){
}

