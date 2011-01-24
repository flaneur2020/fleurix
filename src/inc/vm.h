#ifndef VM_H
#define VM_H

/*
 * each proc got one struct vm, which indicated it's page directory 
 * and misc on address space.
 * */
struct vm {
    struct pde  *vm_pgd;
};

/*
 * proc0's page directory, initialize the kernel's address space here.
 * */
extern struct pde pgd0[];

#endif
