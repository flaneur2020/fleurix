/* some codes are copied from Linux-0.11, thank you linus.
 * */

#define RTC_SEC      0
#define RTC_MIN      2
#define RTC_HOUR     4
#define RTC_MDAY     7
#define RTC_MONTH    8
#define RTC_YEAR     9
#define RTC_CENTURY  0x32
#define RTC_STAT_A   0xA
#define RTC_STAT_B   0xB

#define BCD_TO_BIN(val) (((val)&15) + ((val)>>4)*10)

#define MINUTE 60
#define HOUR  (60*MINUTE)
#define DAY   (24*HOUR)
#define YEAR  (365*DAY)

static int monthtab[12] = {
    0,
    DAY*(31),
    DAY*(31+29),
    DAY*(31+29+31),
    DAY*(31+29+31+30),
    DAY*(31+29+31+30+31),
    DAY*(31+29+31+30+31+30),
    DAY*(31+29+31+30+31+30+31),
    DAY*(31+29+31+30+31+30+31+31),
    DAY*(31+29+31+30+31+30+31+31+30),
    DAY*(31+29+31+30+31+30+31+31+30+31),
    DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

