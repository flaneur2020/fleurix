#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>
#include <syscall.h>

void main(){
    tty_init();         puts("* init tty\n");
    gdt_init();         puts("* init gdt\n");
    idt_init();         puts("* init idt\n");
    page_init();        puts("* init paging\n");
    timer_init();       puts("* init timer\n");
    sched_init();       puts("* init sched\n");
    // proc[0] arises now
    puts("* init user mode\n");
    asm volatile("sti;");
    umode_init();

    int ret = fork();
    if (ret){
        while(1) asm("int $0x80"::"a"(0),"b"(1));
    }
    else {
        while(1) asm("int $0x80"::"a"(0),"b"(0));
    }

    for(;;);
}

// on Memory
void* memcpy(void *dest, void *src, uint count) {
    char *sp = (char *)src;
    char *dp = (char *)dest;
    int i;
    for (i=0; i<count; i++){
        *dp++ = *sp++;
    }
    return dest;
}

void* memset(void *dest, char val, uint count){
    char *dp = (char *)dest;
    int i;
    for(i=0; i<count; i++){
        *dp++ = val;
    }
    return dest;
}

short* memsetw(short *dest, short val, uint count){
    short *dp = (short *)dest;
    int i;
    for(i=0; i<count; i++){
        *dp++ = val;
    }
    return dest;
}

// on str
uint strlen(char *str){
    char* sp;
    for(sp=str; *sp!='\0'; sp++);
    return sp-str;
}



