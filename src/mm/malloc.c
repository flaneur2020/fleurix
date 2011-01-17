#include <param.h>
#include <x86.h>
#include <proc.h>
#include <proto.h>

#include <mm.h>

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
void* kheap_end = KHEAP;

/*
 * Allocate one physical page and attach it where grows the kernel 
 * heap, and extends the kheap_end. No decrease yet.
 * Have a check of the 4mb boundary, on the edge, reserve the last 
 * page as the middle page table, attach it into EVERY proc's page
 * table.
 * */
void* kbrk(){
}

/* 
 * Allocate one bucket.
 * */
struct bucket *bkalloc(){
}

void* kmalloc(uint size){
}

int kfree(void *addr){
}
