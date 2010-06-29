/**
 *
 * Constants and blah~
 *
 * _Fleurer<me.ssword@gmail.com>
 * */ 

#ifndef PARAM_H
#define PARAM_H

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;

#define NULL ((void*)0)

#define NPROC 127
#define NFRAME ((HI_MEM-LO_MEM)/4096)

#define KERN_CS 0x08
#define KERN_DS 0x10

// Kernel is 1mb, which locates in 0~LO_MEM
// main Mem(which procs live in) is 63mb, locates in LO_MEM~HI_MEM 
#define LO_MEM 0x100000
#define HI_MEM 0x4000000

#endif
