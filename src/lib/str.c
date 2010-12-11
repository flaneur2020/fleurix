#include <param.h>

void* memcpy(void *dest, void *src, uint count) {
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

/* on str */
int strlen(char *str){
    char* sp;
    for(sp=str; *sp!='\0'; sp++);
    return sp-str;
}

/* 
 * note that '\0' is considered to be part of the string. 
 * returns not a number but a pointer.
 * */
char* strchr(const char *str, int c){
    for(; *str!='\0'; str++) {
        if (*str == (char)c) {
            return str;
        }
    }
    if (*str == (char)c) 
        return str;
    return NULL;
}

/* 
 * TODO: debug all these.
 * */
int strcmp(char *s1, char *s2){
    while (*s1 == *s2++)
        if (*s1++ == 0)
            return (0);
    return (*(uchar*)s1 - *(uchar*)(s2-1));
}

int strncmp(char *s1, char* s2, uint n) {
    if (n == 0)
        return 0;
    while(n-- != 0) {
        if (*s1 != *s2++)
            return (*(uchar*)s1 - *(uchar*)(s2-1));
        if (*s1++ == 0)
            break;
    } 
    return 0;
}



