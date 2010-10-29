#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>

#include <buf.h>
#include <conf.h>
#include <hdrw.h>

struct bdevsw   bdevsw[] = {
    { &nulldev, &nulldev, &hd_req, &hdtab }
};

