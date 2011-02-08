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

static uint mode = 0; 

/* translate into flags which indicated the satus of shift, ctrl & alt. */
char shift(char sc){
    char ch = sc & 0x7f;

    if (mode & E0ESC) {
        switch (ch) {
        case 0x1D: return CTRL;
        case 0x38: return ALT;
        }
    }
    else {
        switch(ch) {
        case 0x2A:
        case 0x36: return SHIFT;
        case 0x1D: return CTRL;
        case 0x38: return ALT;
        }
    }
    return 0;
}

/*
 * Each keyboard interrupt came along with a 8bit scan code (via inb(KB_DATA)), if bit 8 is 
 * set, it's releasing a key, else pressing. 
 * */
int do_keybd_intr(struct trap *tf){
    uchar sc, ch, m;
    char *map = keybd_map;

    // got no data
    if ((inb(KB_STAT) & KB_STAT_DIB)==0){
        return -1;
    }
    sc = inb(KB_DATA);

    // ignore capslock yet.
    if ((sc & 0x7f) == 0x3A) 
        return 0;

    // check E0ESC
    if (sc == 0xE0) 
        mode |= E0ESC;

    // check shift, ctrl and alt
    if (m = shift(sc)) {
        if (sc & 0x80) 
            mode &= ~m;
        else 
            mode |= m;
        return 0;
    }
    map = (mode & SHIFT)? shift_map: keybd_map;
    ch = map[sc & 0x7f];

    if (mode & CTRL) {
        switch(ch){
        case 'c': ch = CINTR; 
        case 'd': ch = CEOF;
        case 'x': ch = CKILL;
        case 'q': ch = CSTART;
        case 's': ch = CSTOP;
        case 'z': ch = CSUSP;
        case '\\': ch = CQUIT;
        }
    }

    // on pressed 
    if ((sc & 0x80)==0 && ch!=NULL) {
        tty_input(&tty[0], ch);
    }
    // on released
    else {
        mode &= ~E0ESC;
    }
}

void keybd_init(){
    irq_enable(1);
    set_hwint(IRQ0+1, &do_keybd_intr);
}
