#include <param.h>
#include <unistd.h>

/* printf.c */
/* implement a printf() in the user mode. */

void putch(char c){
    write(1, &c, 1);
}

void puts(char *str){
    int i;
    for(i=0; i<strlen(str); i++){
        putch(str[i]);
    }
}

void printn(uint n, uint b){
    static char *ntab = "0123456789ABCDEF";
    uint a, m;
    if (a = n / b){
        printn(a, b);  
    }
    m = n % b;
    putch( ntab[m] );
}

/* a simpler printk
 * refer to unix v6 
 * */
void printf(char *fmt, ...){
    char c, *s;
    uint *adx = (uint*)(void*)&fmt + 1;
_loop:
    while((c = *fmt++) != '%'){
        if (c == '\0') return;
        putch(c);
    }
    c = *fmt++;
    if (c == 'd' || c == 'l'){
        printn(*adx, 10);
    }
    if (c == 'o' || c == 'x'){
        printn(*adx, c=='o'? 8:16 );
    }
    if (c == 'c') {
        putch(*adx);
    }
    if (c == 's'){
        puts(*adx);
    }
    adx++;
    goto _loop;
}
