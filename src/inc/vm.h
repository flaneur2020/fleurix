#ifndef VM_H
#define VM_H

struct vma {
    uint        vma_type;
    uint        vma_flag;
    uint        vma_begin;
    uint        vma_size;
};

/*
 * each proc got one struct vm, which indicated it's page directory 
 * and misc on address space.
 * */
struct vm {
    struct pde  *vm_pgd;
    struct vma   vm_area[0];
    struct vma   vm_text;
    struct vma   vm_data;
    struct vma   vm_bss;
    struct vma   vm_stack;
};

/*
 * proc0's page directory, initialize the kernel's address space here.
 * */
extern struct pde pgd0[];

#endif
