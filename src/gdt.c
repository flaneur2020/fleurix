#include <param.h>
#include <sys.h>
#include <x86.h>

struct seg_desc     gdt[3 + (NPROC+1)*2] = {0, };
struct gdt_desc     gdt_desc;

void set_seg(struct seg_desc *seg, uint base, uint limit, uint dpl, uint type){
    seg->limit_lo = ((limit) >> 12) & 0xffff;
    seg->base_lo  = (base) & 0xffff;
    seg->base_mi  = ((base) >> 16) & 0xff;
    seg->type     = type;
    seg->s        = 1;
    seg->dpl      = dpl;
    seg->present  = 1; 
    seg->limit_hi = (uint) (limit) >> 28;
    seg->avl      = 0;
    seg->r        = 0;
    seg->db       = 1;
    seg->g        = 1;                    \
    seg->base_hi  = (base) >> 24;
}

/******************************************************/

// refill the gdt
// each proc have one idt and tss, which all masses here
void gdt_init(){
    set_seg(&gdt[1], 0, 0xffffffff, 0, STA_X | STA_R);
    set_seg(&gdt[2], 0, 0xffffffff, 0, STA_W);
    gdt_desc.base   = &gdt;
    gdt_desc.limit  = (sizeof (struct seg_desc) * 5) - 1;
    asm volatile( "lgdt %0" :: "m"(gdt_desc));
}

