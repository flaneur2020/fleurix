#include <sys.h>

int main(){
    init_video();
    puts("Fleurix initalizing, welcome\n");
    puts("* init video");
    init_idt();
    puts("* init idt");
    // forerver loop
	for (;;);
}

// on Memory
void* memcpy(void *dest, void *src, size_t count){
    char *sp = (char *)src;
    char *dp = (char *)dest;
    int i;
    for (i=0; i<count; i++){
        *dp++ = *sp++;
    }
    return dest;
}

void* memset(void *dest, char val, size_t count){
    char *dp = (char *)dest;
    int i;
    for(i=0; i<count; i++){
        *dp++ = val;
    }
    return dest;
}

short* memsetw(short *dest, short val, size_t count){
    short *dp = (short *)dest;
    int i;
    for(i=0; i<count; i++){
        *dp++ = val;
    }
    return dest;
}

// on str
size_t strlen(char *str){
    char* sp;
    for(sp=str; *sp!='\0'; sp++);
    return sp-str;
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

void lidt(struct idt_desc* idt_desc){
    asm volatile(
        "lidt %0"
        :: "m"(idt_desc));
}
