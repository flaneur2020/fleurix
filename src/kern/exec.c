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
 * the user stack initialized like this:
 *   |--------------- esp
 *   | argc
 *   |---------------
 *   | argv
 *   |---------------
 *   | argv[0] "..."
 *   | argv[1] "..."
 *   | ...
 *   | argv[n] "..."
 *   --------------- VM_STACK
 * note: ignored envp yet.
 * */
int do_exec(char *path, char **argv){
    struct inode *ip;
    struct buf *bp;
    struct ahead *ah;
    struct page *pg;
    struct vm *vm;
    struct vma *vp;
    struct file *fp;
    uint bn, fd, argc, esp;
    uint base, text, data, bss, heap;
    char *argv0, **uargv;

    ip = namei(path, 0);
    if (ip==NULL) {
        syserr(ENOENT);
        goto _badf;
    }
    // read the first block of file to get the a.out header.
    bn = bmap(ip, 0, 0);
    if (bn == 0) {
        syserr(EINVAL);
        goto _badf;
    }
    bp = bread(ip->i_dev, bn);
    ah = (struct ahead*)bp->b_data;
    // check this a.out header.
    if (ah->a_magic != NMAGIC) {
        syserr(EINVAL);
        goto _badf;
    }
    // dettach the previous address space, and initialize a new one
    vm = &cu->p_vm;
    vm_clear(vm);
    vm_renew(vm, ah, ip);
    // push arguments to the end of user stack, which always the same address.
    esp = VM_STACK;
    argc  = upush_argv(&esp, path, argv);
    if (argc<0)
        panic("exec(): bad mem");
    upush(&esp, &uargv, sizeof(uint));
    upush(&esp, &argc, sizeof(uint));
    // close all the file descriptors with FD_CLOEXEC
    for (fd=0; fd<NOFILE; fd++) {
        fp = cu->p_ofile[fd];
        if (fp!=NULL && (fp->f_fdflag & FD_CLOEXEC)) {
            do_close(fd);
        }
    }
    // never forget this:
    brelse(bp);
    iput(ip);
    // enter user mode
    _retu(vm->vm_entry, esp);
    return 0;

_badf:
    brelse(bp);
    iput(ip);
    return NULL;
}

/* push argv into user stack, returns argc.
 * */
int upush_argv(uint *esp, char *path, char **argv){
    uint arglen, argc;
    int i,r;
    char *str, **uargv, **tmp;

    if (argv==NULL) {
        return 0;
    }
    argc = 1;
    arglen = strlen(path)+1;
    for (i=0; (str=argv[i])!=NULL; i++) {
        arglen += strlen(str)+1;
        argc++;
    }
    arglen += sizeof(char*) * argc;
    if (vm_verify(*esp-arglen, arglen) < 0){
        syserr(EINVAL);
        return -1;
    }
    // store the argv in temp
    tmp    = (char**)kmalloc(PAGE);
    tmp[0] = (char*) kmalloc(PAGE);
    tmp[PAGE-1] = '\0';
    strncpy(tmp[0], path, PAGE-1);
    for(i=1; i<argc; i++) {
        tmp[i] = (char*)kmalloc(PAGE);
        tmp[PAGE-1] = '\0';
        strncpy(tmp[i], argv[i], PAGE-1);
    }
    // push to ustack finally
    uargv = *esp - arglen;
    for (i=argc-1; i>=0; i--){
        str = tmp[i];
        upush(esp, str, strlen(str)+1);
        uargv[i] = (char *) *esp;
    }
    *esp = uargv;
    // freeing tmp
    for(i=0; i<argc; i++){
        kfree(tmp[i], PAGE);
    }
    kfree(tmp, PAGE);
    return argc;
}

/* push one string into the user stack. returns the new esp */
int upush(uint *esp, char *buf, int len){
    uint tmp = *esp; // take care, *esp may overlaps *buf
    if (vm_verify(tmp-=len, len) < 0) {
        panic("upush(): bad mem");
    }
    memcpy(tmp, buf, len);
    return (*esp=tmp);
}

/* ---------------------------------------------------- */

/* for debug */
int dump_ahead(struct ahead *ah){
    printk("sizeof(struct ahead): %x\n", sizeof(struct ahead));
    printk("a_magic: %x\n", ah->a_magic);
    printk("a_tsize: %x\n", ah->a_tsize);
    printk("a_dsize: %x\n", ah->a_dsize);
    printk("a_bsize: %x\n", ah->a_bsize);
    printk("a_syms: %x\n", ah->a_syms);
    printk("a_entry: %x\n", ah->a_entry);
    printk("a_trsize: %x\n", ah->a_trsize);
    printk("a_drsize: %x\n", ah->a_drsize);
}
