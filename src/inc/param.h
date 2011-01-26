typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;

#define PBSIZE  512         /* physical block size */
#define BSIZE   1024        /* logical block size */
#define PAGE    0x1000

#define PMEM     0x8000000  /* 128mb physical memory. */
#define KMEM_END 0x40000000 /* 512mb, the kernel's address space. */
#define KHEAP    0x300000   /* kernel's heap starts at 3mb. */

#define ROOTINO 1

#define NSYSC   64
#define NPROC   64
#define NSEG    6
#define NBUF    (0x200000/BSIZE) /* buffers take 2mb */
#define NBLKDEV 2
#define NINODE  128
#define NFILE   128
#define NOFILE  16
#define NMOUNT  16
#define NKPAGE  (LO_MEM/PAGE)
#define NPAGE   (PMEM/PAGE)

#define NINDBLK (BSIZE/sizeof(ushort))
#define MAX_FILESIZ ((7+NINDBLK+NINDBLK*NINDBLK)*BSIZE)

#define NULL ((void*)0)
#define NUL  0

extern char __kend__;

#include <lib.h>
