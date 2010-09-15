
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>

char user_stack[1024];

void main(){
    video_init();       puts("* init video\n");
    idt_init();         puts("* init idt\n");
    gdt_init();         puts("* init gdt\n");
    page_init();        puts("* init paging\n");
    timer_init(1000);   puts("* init timer\n");
    sched_init();       puts("* init sched\n");
   asm volatile("  \ 
     cli; \ 
     mov $0x23, %ax; \ 
     mov %ax, %ds; \ 
     mov %ax, %es; \ 
     mov %ax, %fs; \ 
     mov %ax, %gs; \ 
                   \ 
     mov %esp, %eax; \ 
     pushl $0x23; \ 
     pushl %eax; \ 
     pushf; \ 
     pushl $0x1B; \ 
     push $1f; \ 
     iret; \ 
   1: \ 
     ");

    //fork();

    //debug_proc_list();

    //panic("`");

    //asm("sti");
    //puts("\nHello, Fleurix... \n\n");

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
}

void debug_eip(){
    uint eip;
    asm("popl %0":"=a"(eip));
    asm("pushl %0"::"a"(eip));
    printf("%x\n", eip);
}


