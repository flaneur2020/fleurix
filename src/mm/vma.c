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

int vma_init(struct vma *vp, uint base, uint size, struct inode *ip, uint ioff){
    vp->v_base = base;
    vp->v_size = size;
    vp->v_ino  = ip;
    vp->v_ioff = ioff;
}
