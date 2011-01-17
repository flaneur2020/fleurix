typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;

#define PBSIZE  512         /* physical block size */
#define BSIZE   1024        /* logical block size */
#define PAGE    0x1000
#define PROCSIZ 0x4000000
#define LO_MEM  0x300000    /* un-mappable memory */
#define HI_MEM  0x1000000   /* 128mb physical memory availible */

#define KHEAP    ((void*)LO_MEM) /* kernel's heap starts at 4mb. */
#define KMEM_END 0x4000000 /* kernel's address space. */

#define ROOTINO 1

#define NSYSC   64
#define NPROC   64
#define NSEG    (4+NPROC+1)
#define NBUF    (0x200000/BSIZE) /* buffers take 2mb */
#define NBLKDEV 2
#define NINODE  128
#define NFILE   128
#define NOFILE  16
#define NMOUNT  16
#define NKPAGE  (LO_MEM/PAGE)
#define NPAGE   ((HI_MEM-LO_MEM)/PAGE)

#define NINDBLK (BSIZE/sizeof(ushort))
#define MAX_FILESIZ ((7+NINDBLK+NINDBLK*NINDBLK)*BSIZE)

#define NULL ((void*)0)

#include <lib.h>
