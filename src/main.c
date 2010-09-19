#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>
#include <syscall.h>

void main(){
    video_init();       puts("* init video\n");
    gdt_init();         puts("* init gdt\n");
    idt_init();         puts("* init idt\n");
    page_init();        puts("* init paging\n");
    timer_init(1000);   puts("* init timer\n");
    sched_init();       puts("* init sched\n");
    //asm("int $0x80"::"a"(0));
    // proc[0] arises now
    puts("* init user mode");
    //asm volatile("sti;");
    umode_init();       
    asm("int $0x80"::"a"(1));
    
    for(;;);

    //panic("`");

    //asm("sti");
    //puts("\nHello, Fleurix... \n\n");
}

// on Memory
void* memcpy(void *dest, void *src, uint count){
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

// panic
void panic(char *str){
    printf("%s\n", str);
    asm volatile("cli");
    asm volatile("hlt");
}

void debug_eip(){
    uint eip;
    asm("popl %0":"=a"(eip));
    asm("pushl %0"::"a"(eip));
    printf("%x\n", eip);
}


