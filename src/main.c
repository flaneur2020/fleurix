#include <sys.h>
#include <x86.h>

void main(){
    video_init();       puts("* init video\n");
    idt_init();         puts("* init idt\n");
    //printf("%d\n", 1/0);
    timer_init(100);    puts("* init timer\n");
    page_init();        puts("* init paging\n");
    puts("\nHello, Fleurix... \n\n");
    asm volatile("sti");
    int i;
    for(i=0; i<NFRAME; i++){
        uint addr = palloc();
        printf("allocated %x\n", addr);
        addr = pfree(addr);
        printf("freed %x\n", addr);
    }
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

