
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>

void main(){
    video_init();       puts("* init video\n");
    idt_init();         puts("* init idt\n");
    gdt_init();         puts("* init gdt\n");
    timer_init(1000);   puts("* init timer\n");
    page_init();        puts("* init paging\n");
    sched_init();       puts("* init sched\n");
    puts("\nHello, Fleurix... \n\n");
    
    asm volatile("sti");

    copy_ptab(0, 0x4000000, 0x10000);

    //blah();

    uint p=0x4000000;
    printf("%d\n", *((uint*)(void*)(0x1000)));
    printf("%x\n", la2pa((p+0x1000)));
    printf("%d\n", *((uint*)(void*)(p+0x100)));

    /*
    if (0==fork()){
        printf("A\n");
        for(;;);
    }
    else{
        printf("B\n");
        for(;;);
    }
    */

    nop();


    //printf("%d\n", 1/0);
    //fork();
    // for debug
    //ljmp(0x08, &nop);
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
    printf("------------------------\n");
}
