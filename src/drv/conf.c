#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <buf.h>
#include <conf.h>
#include <hd.h>

/* TODO: leave the first entry zero, hence NODEV may equals 0 */
struct bdevsw   bdevsw[NDEV] = {
    { 0, },
    { &nulldev, &nulldev, &hd_request, &hdtab },
    { 0, },
};

