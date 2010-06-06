#include <sys.h>

int main(){
    cls();
	char *str = "Hello, world", *ch;
	short *vidmem = (short*) 0xb8000;
	int i;
	
	for (ch = str, i = 0; *ch; ch++, i++)
		vidmem[i] = (char) *ch | 0x0700;
		
	for (;;)
		;
}


void memcpy(void *dest, void *src, unsigned int count){
    
}
