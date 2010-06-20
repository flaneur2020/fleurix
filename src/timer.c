#include <sys.h>

static uint tick = 0;

void do_timer(){
    //puts("~~");
}

void timer_init(uint freq){
    uint di = 1193180/freq;
    outb(0x43, 0x36);
    outb(0x40, (uchar)di&0xff);
    outb(0x40, (uchar)di>>8);
    outb(0x21, inb(0x21)&0xfe);

    /* Installs 'timer_handler' to IRQ0 */
    int_set_handler(IRQ0+0, do_timer);
}

