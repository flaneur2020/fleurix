#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>
#include <cmos.h>

#define HZ 100

static volatile uint tick = 0;
static volatile uint timestamp = 0;

/*****************************************************/

/* read/write RTC */
char cmos_read(char port){
    outb(0x70, port&0x7F);
    return BCD_TO_BIN(inb(0x71));
}

void cmos_write(char port, char val){
    outb(0x70, port&0x7F);
    outb(0x71, BCD_TO_BIN(val));
}

/* get system time */
uint cmos_time(){
    int year, month, mday, hour, min, sec;
    uint r;
    year  = cmos_read(RTC_CENTURY)*100 + cmos_read(RTC_YEAR) - 1970; // start at UNIX era
    month = cmos_read(RTC_MONTH); 
    mday  = cmos_read(RTC_MDAY);
    hour  = cmos_read(RTC_HOUR);
    min   = cmos_read(RTC_MIN);
    sec   = cmos_read(RTC_SEC);
    // magic offsets (y+1) needed to get leapyears right.
    r = YEAR*year + DAY*((year+1)/4);
    r += monthtab[month-1];
    // and (y+2) here. If it wasn't a leap-year, we have to adjust.
    if (month>1 && ((year+2)%4)){
        r -= DAY;
    }
    r += DAY*(mday-1);
    r += HOUR*hour;
    r += MINUTE*min;
    r += sec;
    return r;
}

uint time(){
    return timestamp;
}

/*****************************************************/

void do_timer(struct trap *tf){
    tick++;
    // on shedule
    if (current->p_cpu+1 <= 127) {
        current->p_cpu++;
    }

    if (tick % 30==0) {
        swtch();
    }

    if (tick % HZ ==0){
        sched_cpu();
        timestamp++;
    }
}

/* Initialize PIC and CMOS time. */
void timer_init(){
    uint di = 1193180/HZ;
    outb(0x43, 0x36);
    outb(0x40, (uchar)(di&0xff));
    outb(0x40, (uchar)(di>>8));
    //
    timestamp = cmos_time();
    //
    irq_enable(0);
    set_hwint(IRQ0+0, &do_timer);
    // printf("current cmos time: %d\n", cmos_time());
}

