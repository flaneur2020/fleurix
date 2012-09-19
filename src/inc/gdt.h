/* This file is modified from xv6 */

struct seg_desc {
    uint        limit_lo:16;    // Low bits of segment limit
    uint        base_lo :16;    // Low bits of segment base address
    uint        base_mi :8;     // Middle bits of segment base address
    uint        type    :4;     // Segment type (see STS_ constants)
    uint        s       :1;     // 0 = system, 1 = application
    uint        dpl     :2;     // Descriptor Privilege Level
    uint        present :1;     // Present
    uint        limit_hi:4;     // High bits of segment limit
    uint        avl     :1;     // Unused (available for software use)
    uint        r       :1;     // Reserved
    uint        db      :1;     // 0 = 16-bit segment, 1 = 32-bit segment
    uint        g       :1;     // Granularity: limit scaled by 4K when set
    uint        base_hi :8;     // High bits of segment base address
};

struct gdt_desc {
    ushort      limit;
    uint        base;
} __attribute__((packed));

// Application segment type bits
#define STA_X       0x8     // Executable segment
#define STA_E       0x4     // Expand down (non-executable segments)
#define STA_C       0x4     // Conforming code segment (executable only)
#define STA_W       0x2     // Writeable (non-executable segments)
#define STA_R       0x2     // Readable (executable segments)
#define STA_A       0x1     // Accessed

// System segment type bits
#define STS_LDT     0x2     // Local Descriptor Table
#define STS_TG      0x5     // Task Gate / Coum Transmitions
#define STS_TA      0x9     // Available 32-bit TSS
#define STS_TB      0xB     // Busy 32-bit TSS
#define STS_CG      0xC     // 32-bit Call Gate
#define STS_IG      0xE     // 32-bit Interrupt Gate
#define STS_TRG     0xF     // 32-bit Trap Gate

/* selectors */
#define KERN_CS (1<<3)
#define KERN_DS (2<<3)
#define USER_CS ((3<<3)|3)
#define USER_DS ((4<<3)|3)

#define TSS0 5
#define _TSS (TSS0<<3)
#define LDT0 6
#define _LDT(n) ((LDT0+n)<<3)
