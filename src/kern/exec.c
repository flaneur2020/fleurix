#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <unistd.h>
//
#include <page.h>
#include <vm.h>
//
#include <buf.h>
#include <conf.h>
#include <hd.h>
//
#include <tty.h>
//
#include <super.h>
#include <inode.h>
#include <file.h>
// 
#include <a.out.h>

char* upush(char* esp, char *buf, int len);

/*
 * exec.c
 *
 * this file indicated the implementation of exec(char* path, char **argv).
 * exec() create a NEW address space, which overlapped the old forked address 
 * space, make it demand-reading.
 * */


/* initialize a new struct vm according to an a.out executable 
 * image.
 * returns NULL on fail.
 * TODO: ignored envp yet.
 * */
int do_exec(char *path, char **argv){
    struct inode *ip;
    struct buf *bp;
    struct ahead *ah;
    struct page *pg;
    int bn;
    struct vm *vm;
    struct vma *vp;
    uint base, text, data, bss, heap;
    char* esp;

    ip = namei(path, 0);
    if (ip==NULL) 
        goto _badf;
    // read the first block of file to get the a.out header.
    bn = bmap(ip, 0, 0);
    if (bn == 0) {
        goto _badf;
    }
    bp = bread(ip->i_dev, bn);
    ah = (struct ahead*)bp->b_data;
    // check this a.out header.
    if (ah->a_magic != NMAGIC) 
        goto _badf;
    // 
    base = ah->a_entry - sizeof(struct ahead); // note: keep alignment
    text = ah->a_entry;
    data = text + ah->a_tsize;
    bss  = data + ah->a_dsize;
    heap = bss  + ah->a_bsize;
    // initialize all the VMAs
    vm = &cu->p_vm;
    vma_init(&(vm->vm_text),  text,  ah->a_tsize, VMA_MMAP, ip, text-base);
    vma_init(&(vm->vm_data),  data,  ah->a_dsize, VMA_MMAP, ip, data-base);
    vma_init(&(vm->vm_bss),   bss,   ah->a_bsize, VMA_ZERO, NULL, NULL);
    vma_init(&(vm->vm_heap),  heap,  PAGE,        VMA_ZERO, NULL, NULL);
    vma_init(&(vm->vm_stack), VM_STACK, PAGE,     VMA_STACK|VMA_ZERO, NULL, NULL);
    // give a page to user stack
    pg = pgalloc();
    pgattach(vm->vm_pgd, VM_STACK-PAGE, pg, PTE_W|PTE_U|PTE_P);
    //
    esp = (char*)VM_STACK;
    char *str = "hello";
    esp = upush(esp, str, strlen(str)+1);
    esp = upush(esp, str, strlen(str)+1);
    printf("esp: %x\n", esp);
    printf("esp: %s\n", esp);

    dump_ahead(ah);
    
    unlk_ino(ip);
    return 0;

_badf:
    unlk_ino(ip);
    return NULL;
}

/* push one string into the user stack. returns the new esp */
char* upush(char* esp, char *buf, int len){
    vm_verify(esp-len, len);
    esp -= len;
    memcpy(esp, buf, len);
    return esp;
}

/* ---------------------------------------------------- */

/* for debug */
int dump_ahead(struct ahead *ah){
    printf("sizeof(struct ahead): %x\n", sizeof(struct ahead));
    printf("a_magic: %x\n", ah->a_magic);
    printf("a_tsize: %x\n", ah->a_tsize);
    printf("a_dsize: %x\n", ah->a_dsize);
    printf("a_bsize: %x\n", ah->a_bsize);
    printf("a_syms: %x\n", ah->a_syms);
    printf("a_entry: %x\n", ah->a_entry);
    printf("a_trsize: %x\n", ah->a_trsize);
    printf("a_drsize: %x\n", ah->a_drsize);
}
