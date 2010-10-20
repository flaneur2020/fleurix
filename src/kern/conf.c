#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>
#include <buf.h>
#include <conf.h>

struct bdevsw   bdevsw[] = {
    { &nulldev, &nulldev, &hdreq, &hdtab }
};


