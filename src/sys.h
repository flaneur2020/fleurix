/**
 *
 * just misc right now
 *
 * */ 

typedef unsigned int    size_t;

// in main.c
// on mem
void *memcpy(void *dest, void *src, size_t count);
void *memset(void *dest, char val, size_t count);
short *memsetw(short *dest, short val, size_t count);


// in video.c
void    init_video();
void    cls();
void    putch(char ch);
void    puts(char *ch);
void    printf();

// on x86
// in & out
static inline char port_inb(short _port){
    char r;
    __asm __volatile(
        "inb %1, %0" :
        "=a" (r) : "dN" (_port));
    return r;
}
static inline void port_outb(short _port, char _data){
    __asm __volatile(
        "outb %1, %0"
        : : "dN" (_port), "a" (_data));
}
