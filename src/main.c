#include <sys.h>

void main(){
    video_init();
    puts("Fleurix initalizing, welcome\n\n");
    puts("* init video\n");
    idt_init();
    puts("* init idt\n");
    timer_init(100, NULL);
    asm volatile("sti");
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

// x86 
char inb(short port){
    char ret;
    asm volatile(
        "inb %1, %0"
        : "=a" (ret) 
        : "dN" (port));
    return ret;
}

void outb(short port, char data){
    asm volatile(
        "outb %1, %0"
        :: "dN" (port), "a" (data));
}


