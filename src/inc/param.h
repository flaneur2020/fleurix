#define PHYBLK  512
#define BLK     1024
#define PAGE    0x1000
#define PROCSIZ 0x400000
#define LO_MEM  0x300000  /* kernel resides in 0~LO_MEM */
#define HI_MEM  0x1000000 /* total 128mb physical memory */
#define ROOTINO 1
#define NAMELEN 14

#define NSYSC   64
#define NPROC   64
#define NSEG    (4+NPROC+1)
#define NBUF    (0x200000/BLK) /* buffers take 2mb */
#define NBLKDEV 2
#define NINODE  128
#define NFILE   128
#define NOFILE  16
#define NMOUNT  16
#define NKPAGE  (LO_MEM/PAGE)
#define NPAGE   (HI_MEM/PAGE)

#include <lib.h>
