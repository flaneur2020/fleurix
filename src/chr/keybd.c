#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <tty.h>
#include <keybd.h>

/*
 * keybd.c
 *
 * ps: this article is marvulous.
 * http://www.osdever.net/papers/view/ibm-pc-keyboard-information-for-software-developers
 *
 * */

/*
 * Each keyboard interrupt came along with a 8bit scan code (via inb(KB_DATA)), if bit 8 is 
 * set, it's releasing a key, else pressing. 
 * */
int do_keybd_intr(struct trap *tf){
    uchar sc, ch;

    // got no data
    if ((inb(KB_STAT) & KB_STAT_DIB)==0){
        return -1;
    }
    sc = inb(KB_DATA);
    // it's released
    if (sc & 0x80) {
        return 0;
    }
    ch = sc & 0x7f;
    putch(keybd_map[ch]);
}

void keybd_init(){
    irq_enable(1);
    set_hwint(IRQ0+1, &do_keybd_intr);
}
