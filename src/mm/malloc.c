#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <page.h>
#include <vm.h>
#include <malloc.h>

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
struct bktab bktab[] = {
    {32, NULL}, 
    {64, NULL}, 
    {128, NULL}, 
    {256, NULL}, 
    {512, NULL}, 
    {1024, NULL}, 
    {2048, NULL}, 
    {4096, NULL}
};

struct bucket bkfreelist = {0, };

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
    struct pte *pgdir, pde, npde, *pmd, *pte;
    struct page *pp;
    struct proc *p;
    int i;
    
    pgdir = cu->p_vm.vm_pgdir;
    // if it's the last availible page on the 4mb boundary.
    if (((kheap_end+PAGE) % (PAGE*1024))==0){
        // if the next pde is not present
        npde = pgdir[PDX(kheap_end+PAGE)];
        if ((npde.pt_flag & PTE_P)==0) {
            // allocate attach the physical page to the 4mb boundary
            pde = pgdir[PDX(kheap_end)];
            pmd = (struct pte*)(pde.pt_num << 12);
            pte = &pmd[PTX(kheap_end)];
            if ((pte->pt_flag & PTE_P)==0) {
                pp = pgalloc();
                pte->pt_num = pp->pg_num;
                pte->pt_flag = PTE_P | PTE_W;
                lpgdir(pgdir);
            }
            npde.pt_num = PPN(kheap_end);
            npde.pt_flag = PTE_P | PTE_W;
            pgdir[PDX(kheap_end+PAGE)] = npde;
            lpgdir(pgdir);
            //
            kheap_end += PAGE;
            // update each proc's page table
            for (i=0; i<NPROC; i++) {
                if ((p=proc[i])!=NULL && p->p_vm.vm_pgdir!=NULL) {
                    p->p_vm.vm_pgdir[PDX(kheap_end)] = pde;
                }
            }
        }
    }
    pte = find_pte(pgdir, kheap_end+PAGE);
    if ((pte->pt_flag & PTE_P)==0) {
        pp = pgalloc();
        pte->pt_num = pp->pg_num;
        pte->pt_flag = PTE_P | PTE_W;
        lpgdir(pgdir);
    }
    kheap_end += PAGE;
    return (kheap_end-PAGE);
}

/* ---------------------------------------------------------- */

/* 
 * Allocate one bucket. All buckets are 
 * */
struct bucket *bkalloc(){
    struct bucket *bk, *bh;
    uint page, page_end;

    if (bkfreelist.bk_next == NULL) {
        cli();
        bh = &bkfreelist;
        page = kbrk();
        page_end = page + PAGE;
        for (bk = (struct bucket *)page; (uint)bk < page_end; bk++) {
            bh->bk_next = bk;
            bk->bk_next = NULL;
            bh = bk;
        }
        sti();
    }
    // take the head.
    bk = bkfreelist.bk_next;
    bkfreelist.bk_next = bk->bk_next;
    return bk;
}

int bkfree(struct bucket *bk){
    cli();
    bk->bk_next = bkfreelist.bk_next;
    bkfreelist.bk_next = bk;
    sti();
}

/* -------------------------------------------------------- */

void* kmalloc(uint size){
}

int kfree(void *addr){
}
