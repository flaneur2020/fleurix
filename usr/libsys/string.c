#include <lib.h>

void* memcpy(void *dest, void *src, unsigned int count) {
    char *sp = (char *)src;
    char *dp = (char *)dest;
    int i;
    for (i=0; i<count; i++){
        *dp++ = *sp++;
    }
    return dest;
}

void* memset(void *dest, char val, unsigned int count){
    char *dp = (char *)dest;
    int i;
    for(i=0; i<count; i++){
        *dp++ = val;
    }
    return dest;
}

short* memsetw(short *dest, short val, unsigned int count){
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

int strnlen(char *str, unsigned int len){
    char *sp;
    for (sp=str; *sp != '\0' && sp < str+len; sp++);
    return sp-str;
}

/* ------------------------------------------------------ */

char* strcpy(char *dst, const char *src) {
	char *tmp = dst;
    while (*dst++ = *src++); 
	return tmp;
}

char *strncpy(char *dst, const char *src, unsigned int cnt) {
    char *tmp = dst;
    while (cnt && (*dst++ = *src++)) 
        cnt--;
    if (cnt > 0) 
        while (--cnt) 
            *dst++ = '\0';
    return tmp;
}

char *strcat(char *dst, const char *src){
    char *tmp = dst;
    for(;*tmp; tmp++);
    while (*tmp++ = *src++);
    return dst;
}

char *strncat(char *dst, const char *src, unsigned int n){
    char *tmp = dst;
    if (n==0) 
        return dst;
    for(;*tmp; tmp++);
    while (*tmp++ = *src++) {
        if (--n <= 0) {
            *tmp = '\0';
            break;
        }
    }
    return dst;
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

/* just like strchr, but in reverse order. */
char* strrchr(const char *str, int c){
    char *ret;
    for(; *str!='\0'; str++){
        if (*str == (char)c) {
            ret = str;
        }
    }
    if (*ret == (char)c)
        return ret;
    return NULL;
}

/* 
 * TODO: debug all these.
 * */
int strcmp(char *s1, char *s2){
    while (*s1 == *s2++)
        if (*s1++ == 0)
            return (0);
    return (*(unsigned char*)s1 - *(unsigned char*)(s2-1));
}

int strncmp(char *s1, char* s2, unsigned int n) {
    if (n == 0)
        return 0;
    while(n-- != 0) {
        if (*s1 != *s2++)
            return (*(unsigned char*)s1 - *(unsigned char*)(s2-1));
        if (*s1++ == 0)
            break;
    } 
    return 0;
}
