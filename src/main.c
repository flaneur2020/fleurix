
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>

void main(){
    video_init();       puts("* init video\n");
    idt_init();         puts("* init idt\n");
    nop();
    gdt_init();         puts("* init gdt\n");
    timer_init(100);    puts("* init timer\n");
    page_init();        puts("* init paging\n");
    sched_init();       puts("* init sched\n");
    puts("\nHello, Fleurix... \n\n");
    
    asm volatile("sti");
    asm("int $0x80"::"a"(0));
    //printf("%d\n", 1/0);
    //fork();
    // for debug
	for (;;);
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
    for(;;);
}

void nop(){
}
