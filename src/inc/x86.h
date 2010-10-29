/*
 * Misc on x86
 * constants and structures for so many descriptors
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
#define STS_TRG     0xF     // 32-bit Trap Gate

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
#define PDX(la)  ((uint)((la>>22)&0x3FF))
#define PTX(la)  ((uint)((la>>12)&0x3FF))
#define POFF(la) ((uint)((la&0xFFF)))
#define PPN(la)  (((uint) (la)) >> 12)

#define PTE_ADDR(pte)	((uint)(pte) & ~0xFFF)

// Page table/directory entry flags.
#define PTE_P		0x001	// Present
#define PTE_W		0x002	// Writeable
#define PTE_U		0x004	// User
#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable
#define PTE_A		0x020	// Accessed
#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_MBZ		0x180	// Bits must be zero

// Page Fault error code flags
#define PFE_P       0x001   // Causing the fault was not a Present page
#define PFE_W       0x002   // Causing the fault was on a Wirite
#define PFE_U       0x004   // The fault was caused in User mode
#define PFE_RSVD    0x008   // The Fault was caused by reserved bits set to 1 in a page directory
#define PFE_I       0x010   // The Fault was caused on a instruction fetch.

// Control Register flags
#define CR0_PE		0x00000001	// Protection Enable
#define CR0_MP		0x00000002	// Monitor coProcessor
#define CR0_EM		0x00000004	// Emulation
#define CR0_TS		0x00000008	// Task Switched
#define CR0_ET		0x00000010	// Extension Type
#define CR0_NE		0x00000020	// Numeric Errror
#define CR0_WP		0x00010000	// Write Protect
#define CR0_AM		0x00040000	// Alignment Mask
#define CR0_NW		0x20000000	// Not Writethrough
#define CR0_CD		0x40000000	// Cache Disable
#define CR0_PG		0x80000000	// Paging

/********************************************************************************/

/* 
 * on trap
 * idt and trap frame
 */

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

struct trap_frame {
    uint        gs, fs, es, ds;                             /* pushed the segs last */
    uint        edi, esi, ebp, esp, ebx, edx, ecx, eax;     /* pushed by 'pusha' */
    uint        int_no, err_code;                           /* our 'push byte #' and ecodes do this */
    uint        eip, cs, eflags, esp3, ss3;                 /* pushed by the processor automatically */ 
} __attribute__((packed));


/*****************************************************************************************************/
// on task switching
// tss

struct tss_desc {
    uint        link, esp0;
    ushort      ss0, __1;
    uint        esp1;
    ushort      ss1, __2;
    uint        esp2;
    ushort      ss2, __3;
    uint        cr3, eip, eflags;
    uint        eax, ecx, edx, ebx, esp, ebp, esi, edi;
    ushort      es, __4;
    ushort      cs, __5;
    ushort      ss, __6;
    ushort      ds, __7;
    ushort      fs, __8;
    ushort      gs, __9;
    ushort      ldt,__10;
    ushort      trap, iomb;
} __attribute__((packed));

extern struct tss_desc tss;

/*****************************************************************************************************/

/* inb & outb */
static inline uchar inb(ushort port){
    uchar ret;
    asm volatile( "inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline void outb(ushort port, uchar data){
    asm volatile( "outb %1, %0" :: "dN" (port), "a" (data));
}

/* inw & outw */
static inline uchar inw(ushort port){
    ushort ret;
    asm volatile( "inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

static inline void outw(ushort port, ushort data){
    asm volatile( "outw %1, %0" :: "dN" (port), "a" (data));
}

/* insb & outsb */
static inline void insb(uint port, void *addr, int cnt) {
	asm volatile(
            "cld;" "repne;" "insb"
			 :"=D" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"memory", "cc");
}

static inline void outsb(uint port, void *addr, int cnt) {
	asm volatile("cld;" "repne;" "outsb"
			 :"=S" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"cc");
}

/* insl & outsl */
static inline void insl(uint port, void *addr, int cnt) {
	asm volatile(
            "cld;" "repne;" "insl"
			 :"=D" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"memory", "cc");
}

static inline void outsl(uint port, void *addr, int cnt) {
	asm volatile("cld;" "repne;" "outsl"
			 :"=S" (addr), "=c" (cnt)
			 :"d" (port), "0" (addr), "1" (cnt)
			 :"cc");
}

/* load TSS into tr */
static inline void ltr(uint n){
    asm volatile("ltr %%ax"::"a"(n));
}

static inline void lldt(uint n){
    asm volatile("lldt %%ax"::"a"(n));
}

/*
 * ljmp seg, offset...Lovely little instruction.
 * But seg seems only availible for immediate value at compile time.
 * Tricks needed, *Sucks*.
 * */
static inline void ljmp(ushort seg, uint offset){
    struct{ uint offset, seg; } _tmp;
    _tmp.offset = offset;
    _tmp.seg    = seg;
    asm volatile(
        "ljmp %0"
        ::"m"(_tmp)
        );
}

static inline void lcall(ushort seg, uint offset){
    struct{ uint offset, seg; } _tmp;
    _tmp.offset = offset;
    _tmp.seg    = seg;
    asm volatile(
        "lcall %0"
        ::"m"(_tmp)
        );
}

#endif

