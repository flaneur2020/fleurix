#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
//
#include <page.h>
#include <vm.h>
//
#include <buf.h>
#include <conf.h>
#include <hd.h>
//
#include <super.h>
#include <inode.h>
#include <file.h>


/*
 * find the vma where the vaddr lied in,
 * returns NULL on fail.
 * */
struct vma* find_vma(uint addr){
    struct vma* vma;
    struct vma* vp;
    //
    vma = cu->p_vm.vm_area;
    for (vp=&vma[0]; vp<&vma[NVMA]; vp++) {
        if (addr >= vp->v_base && addr < vp->v_base+vp->v_size) {
            return vp;
        }
    }
    return NULL;
}

/* */
int vma_init(struct vma *vp, uint base, uint size, uint flag, struct inode *ip, uint ioff){
    vp->v_flag = flag;
    vp->v_base = base;
    vp->v_size = size;
    vp->v_ino  = ip;
    vp->v_ioff = ioff;
}
