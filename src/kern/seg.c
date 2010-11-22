#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

/**
 *  GDT
 *  -----------
 *  0 NULL_SEG
 *  1 KERN_CS
 *  2 KERN_DS
 *  3 TSS
 *  4 LDT0
 *  5 LDT1
 *  6 LDT2
 *  7 LDT3
 *  8 ...
 *  ...
 * */

struct seg_desc     gdt[NSEG] = {0, };
struct gdt_desc     gdt_desc;

/******************************************************************/

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
    seg->g        = 1;
    seg->base_hi  = (base) >> 24;
}

void set_ldt(struct seg_desc *seg, uint base){
    set_seg(seg, base, 0, 0, STS_LDT);
    seg->limit_lo = 0x3;
    seg->s = 0;
}

// sizeof(tss_desc) == 104
void set_tss(struct seg_desc *seg, uint base){
    set_seg(seg, base, 0, 0, STS_TA);
    seg->limit_lo = 0x68;
    seg->s = 0;
}

/******************************************************************/

void set_seg_limit(struct seg_desc *seg, uint limit){
    seg->limit_lo = ((limit) >> 12) & 0xffff;
    seg->limit_hi = (uint) (limit) >> 28;
}

void set_seg_base(struct seg_desc *seg, uint base){
    seg->base_lo  = (base) & 0xffff;
    seg->base_mi  = ((base) >> 16) & 0xff;
    seg->base_hi  = (base) >> 24;
}

uint get_seg_limit(struct seg_desc *seg){
    return (seg->limit_lo | seg->limit_hi << 16)<<12;
}

uint get_seg_base(struct seg_desc *seg){
    return (seg->base_lo | seg->base_mi << 16 | seg->base_hi << 24);
}

/******************************************************************/

/* 
 * refill the gdt(the ex-gdt initialized in boot/boot.S)
 * each proc have one ldt, and shared one tss.
 */
void gdt_init(){
    set_seg(&gdt[1], 0, 0xffffffff, 0, STA_X | STA_R);
    set_seg(&gdt[2], 0, 0xffffffff, 0, STA_W);
    gdt_desc.base   = &gdt;
    gdt_desc.limit  = (sizeof (struct seg_desc) * NSEG) - 1;
    // load gdt
    asm volatile( "lgdt %0" :: "m"(gdt_desc));
}

