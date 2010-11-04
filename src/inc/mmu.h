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
