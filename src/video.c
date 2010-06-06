#include <sys.h>

/*
 * width 80
 * height 25
 *
 * */

#define VID_WHITE 0x07 // white on black text
#define VID_BLANK (' ' | (VID_WHITE << 8)) // white on black text

static short *vidmem = (char *) 0xb8000;
static int   csr_x = 0;
static int   csr_y = 0;


void flush_csr(){
    unsigned pos = csr_y * 80 + csr_x;
    // TODO: bug here
    //       I'm not sure if this fucked in port_outb or elsewhere
    //port_outb(0x3D4, 14);
    //port_outb(0x3D5, pos >> 8);
    //port_outb(0x3D4, 15);
    //port_outb(0x3D5, pos);
}

void cls(){
    int i;
    for(i = 0; i < 25; i++) {
        memsetw (vidmem + i * 80, VID_BLANK, 80);
    }
    csr_x = 0;
    csr_y = 0;
    flush_csr();
}

void scroll(void) {
    if(csr_y >= 25) {
        unsigned pos = csr_y - 25 + 1;
        memcpy(vidmem, vidmem + pos * 80, (25 - pos) * 80 * 2);
        memsetw(vidmem + (25 - pos) * 80, VID_BLANK, 80);
        csr_y = 25 - 1;
    }
}

void putch(char c){
    if(c == '\b') {
        if(csr_x != 0) csr_x--;
    }
    else if(c == '\t') {
        csr_x = (csr_x + 8) & ~(8 - 1);
    }
    else if(c == '\r') {
        csr_x = 0;
    }
    else if(c == '\n') {
        csr_x = 0;
        csr_y++;
    }
    else if(c >= ' ') {
        unsigned *vchar = vidmem + (csr_y * 80 + csr_x);
        *vchar = c | VID_WHITE << 8;
        csr_x++;
    }

    if(csr_x >= 80) {
        csr_x = 0;
        csr_y++;
    }
    scroll();
    flush_csr();
}

void puts(char *str){
    int i;
    for(i=0; i<strlen(str); i++){
        putch(str[i]);
    }
}

void init_video(){
    vidmem = (char *) 0xb8000;
    cls();
}
