#define NSYSC 64
#define NPROC 64
#define NSEG  (4+NPROC+1)

/* buffers take 2mb */
#define NBUF (0x200000/512)

/* kernel locates in 0~LO_MEM */
#define LO_MEM (0x300000)
#define HI_MEM 0x1000000

#define PAGE 0x1000
#define NPAGE ((HI_MEM-LO_MEM)/PAGE)

/* clock intervals HZ times per second*/
#define HZ 10

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;

#define NULL ((void*)0)
