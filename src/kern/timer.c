#include <param.h>
#include <x86.h>
#include <kern.h>

#define HZ 100

static uint tick = 0;

void do_timer(struct regs *r){
    tick++;
    if(tick % 10==0){
        do_sched(r);
    }
}

void timer_init(){
    uint di = 1193180/HZ;
    outb(0x43, 0x36);
    outb(0x40, (uchar)(di&0xff));
    outb(0x40, (uchar)(di>>8));
    outb(0x21, inb(0x21)&0xfe);
}

