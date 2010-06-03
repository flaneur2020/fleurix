
void main(){
	char *str = "Hello, world\n", *ch;
	unsigned short *vidmem = (unsigned short*) 0xb8000;
	unsigned i;
	
	for (ch = str, i = 0; *ch; ch++, i++)
		vidmem[i] = (unsigned char) *ch | 0x0700;
		
	for (;;)
		;
}

void putc(char c){

}
