/* this file is modified from xv6 */

#ifndef MMU_H
#define MMU_H

struct pde {
    uint      pd_flag:9;
    uint      pd_avl:3;
    uint      pd_off:20;
};

struct pte {
    uint      pt_flag:9;
    uint      pt_avl:3;
    uint      pt_off:20;
};

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

/*
 * A linear address 'la' has a three-part structure as follows:
 *
 * +--------10------+-------10-------+---------12----------+
 * | Page Directory |   Page Table   | Offset within Page  |
 * |      Index     |      Index     |                     |
 * +----------------+----------------+---------------------+
 *  \--- PDX(la) --/ \--- PTX(la) --/ \---- POFF(la) ----/
 *  \----------- PPN(la) -----------/
 **/

#define PDX(la)  ((uint)((la>>22)&0x3FF))
#define PTX(la)  ((uint)((la>>12)&0x3FF))
#define POFF(la) ((uint)((la&0xFFF)))
#define PPN(la)  (((uint) (la)) >> 12)

#define PG_ADDR(addr)   ((uint)(addr) & ~0xFFF)

/* CR2 stores the virtual address on which raised the page fault.
 * error code stores inside the trap frame.
 * */

// Page Fault error code flags
#define PFE_P       0x001   // Causing the fault was not a Present page
#define PFE_W       0x002   // Causing the fault was on a Wirite
#define PFE_U       0x004   // The fault was caused in User mode
#define PFE_RSVD    0x008   // The Fault was caused by reserved bits set to 1 in a page directory
#define PFE_I       0x010   // The Fault was caused on a instruction fetch.

#endif
