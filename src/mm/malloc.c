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

void* kheap_end = KHEAP;

/*
 * the bucket table.
 * */
struct bktab bktab[] = {
    32, NULL,
    128, NULL,
    256, NULL,
    512, NULL,
    1024, NULL,
    4096, NULL
};

/*
 * Allocate one physical page and attach it where grows the kernel 
 * heap, and extends the kheap_end. No decrease yet.
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
