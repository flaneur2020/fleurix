#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <vm.h>

/*
 * malloc.c
 * malloc could simplify the implemention of your kernel.
 *
 * However, this routine kmalloc() should returns a *virutal* memory
 * block, whose backend is a page-level allocator -- pgalloc().
 * And the memory block should be aligned with a 2-powered boundary.
 *
 * */

/*
 * the bucket table, each slot got a different 2-powered size. 1 << 12 == 0x1000
 * */
struct bucket *bktab[12];

/*
 * 
 * */
uint kheap_end = KHEAP;

/*
 * The page-level allocator used by kmalloc() and bkalloc(), for 
 * allocating non-reusable pages.
 * Allocate one physical page and attach it where grows the kernel 
 * heap, then extends the kheap_end. No decrease yet.
 * Have a check of the 4mb boundary, reserve the last page as the 
 * middle page table on the edge, attach it into EVERY proc's page
 * table.
 * */
uint kbrk(){
    struct pte *pgdir;
    struct pte *pmd;
    struct page *pp;
    struct proc *p;
    int i;
    
    pgdir = cu->p_vm->vm_pgdir;
    // if it's the last availible page on the 4mb boundary.
    if (((kheap_end+PAGE) % (PAGE*1024))==0){
        pmd = &pgdir[PDX(kheap_end)];
        if ((pmd->pt_flag & PTE_P)==0) {
            pp = pgalloc();
            pmd->pt_num = pp->pg_num;
            pmd->pt_flag = PTE_P | PTE_W;
            lpgdir(pgdir);
            kheap_end += PAGE;
            // update each proc's page table
            for (i=0; i<NPROC; i++) {
                if ((p=proc[i])!=NULL) {
                    p->p_vm->vm_pgdir[PDX(kheap_end)] = *pmd;
                }
            }
        }
    }
    pte = find_pte(kheap_end);
    pp = pgalloc();
    pte->pt_num = pp->pg_num;
    pte->pt_flag = PTE_P | PTE_W;
    kheap_end += PAGE;
    return (kheap_end-PAGE);
}

/* 
 * Allocate one bucket. All buckets are 
 * */
struct bucket *bkalloc(){
}

void* kmalloc(uint size){
}

int kfree(void *addr){
}
