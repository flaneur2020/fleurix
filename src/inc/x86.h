/*
 * Misc on x86
 * Consts on CPU
 * and structures of so many descriptors
 *
 *  _Fleurer<me.ssword@gmail.com>
 * */

#ifndef X86_H
#define X86_H


// Eflags register
#define FL_CF           0x00000001      // Carry Flag
#define FL_PF           0x00000004      // Parity Flag
#define FL_AF           0x00000010      // Auxiliary carry Flag
#define FL_ZF           0x00000040      // Zero Flag
#define FL_SF           0x00000080      // Sign Flag
#define FL_TF           0x00000100      // Trap Flag
#define FL_IF           0x00000200      // Interrupt Enable
#define FL_DF           0x00000400      // Direction Flag
#define FL_OF           0x00000800      // Overflow Flag
#define FL_IOPL_MASK    0x00003000      // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000      //   IOPL == 0
#define FL_IOPL_1       0x00001000      //   IOPL == 1
#define FL_IOPL_2       0x00002000      //   IOPL == 2
#define FL_IOPL_3       0x00003000      //   IOPL == 3
#define FL_NT           0x00004000      // Nested Task
#define FL_RF           0x00010000      // Resume Flag
#define FL_VM           0x00020000      // Virtual 8086 mode
#define FL_AC           0x00040000      // Alignment Check
#define FL_VIF          0x00080000      // Virtual Interrupt Flag
#define FL_VIP          0x00100000      // Virtual Interrupt Pending
#define FL_ID           0x00200000      // ID flag

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
#define STS_TG      0xF     // 32-bit Trap Gate

/**********************************************************************************/
// on MMU
// Seg and Paging

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

// on Paging

struct page_desc {
    uint        present  :1;        // Page present in memory
    uint        rw       :1;        // Read-only if clear, readwrite if set
    uint        user     :1;        // Supervisor level only if clear
    uint        accessed :1;        // Has the page been accessed since last refresh?
    uint        dirty    :1;        // Has the page been written to since last refresh?
    uint        always0  :7;        // Amalgamation of unused and reserved bits
    uint        offset   :20;       // Frame address (shifted right 12 bits)
} __attribute__((packed));

// A linear address 'la' has a three-part structure as follows:
//
// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(la) --/ \--- PTX(la) --/ \---- POFF(la) ----/
//  \----------- PPN(la) -----------/
//
#define PDX(la)  ((uint)((la>>22)))
#define PTX(la)  ((uint)((la<<12)&0x3FF))
#define POFF(la) ((uint)((la&0xFFF)))

/********************************************************************************/
// on Intrupt
// idt and frame

#define IRQ0 32

struct gate_desc {
    uint        base_lo :16;        // low address
    uint        sel     :16;        // selector
    uint        always0 :8;         // reserved
    uint        type    :4;         // type(STS_{TG,IG32,TG32})
    uint        sys     :1;         // must be 0 (system)
    uint        dpl     :2;         // descriptor(meaning new) privilege level
    uint        p       :1;         // Present
    uint        base_hi :16;        // high address
} __attribute__((packed));

struct idt_desc {
    ushort      limit;
    uint        base;
} __attribute__((packed));     

struct regs {
    uint        gs, fs, es, ds;                            /* pushed the segs last */
    uint        edi, esi, ebp, esp, ebx, edx, ecx, eax;    /* pushed by 'pusha' */
    uint        int_no, err_code;                          /* our 'push byte #' and ecodes do this */
    uint        eip, cs, eflags, useresp, ss;              /* pushed by the processor automatically */ 
} __attribute__((packed));     


/*****************************************************************************************************/
static inline uchar inb(ushort port){
    uchar ret;
    asm volatile(
        "inb %1, %0"
        : "=a" (ret) 
        : "dN" (port));
    return ret;
}

static inline void outb(ushort port, uchar data){
    asm volatile(
        "outb %1, %0"
        :: "dN" (port), "a" (data));
}

#endif

