/**
 *
 * just misc right now
 *
 * */ 

typedef unsigned int    size_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;

// In main.c
// on Mem
void *memcpy(void *dest, void *src, size_t count);
void *memset(void *dest, char val, size_t count);
short *memsetw(short *dest, short val, size_t count);
// on String
size_t strlen(char *str);

// In video.c
void    init_video();
void    cls();
void    putch(char ch);
void    puts(char *ch);
void    printf();

// In idt.c
void    init_idt();

// on x86, just inline
// in & out
// on x86, just inline
static inline char inb(short port){
    char ret;
    asm volatile(
        "inb %1, %0"
        : "=a" (ret) 
        : "dN" (port));
    return ret;
}
static inline void outb(short port, char data){
    asm volatile(
        "outb %1, %0"
        :: "dN" (port), "a" (data));
}
