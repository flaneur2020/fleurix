
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>

char user_stack[1024];

void main(){
    video_init();       puts("* init video\n");
    timer_init(1000);   puts("* init timer\n");
    idt_init();         puts("* init idt\n");
    gdt_init();         puts("* init gdt\n");
    page_init();        puts("* init paging\n");
    // move stack into a touchable place
    asm volatile("mov %0, %%esp;"::"a"(user_stack+0x1000));
    sched_init();       puts("* init sched\n");
    // proc[0] arises now
    umode_init();       puts("* init user mode\n");

    for(;;);

    //debug_proc_list();

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


