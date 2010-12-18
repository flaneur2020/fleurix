#ifndef LIB_H
#define LIB_H

/* prototypes */

/* string */
void*   memcpy(void *dest, void *src, uint count);
void*   memset(void *dest, char val, uint count);
short*  memsetw(short *dest, short val, uint count);
int     strlen(char *str);
char*   strchr(const char *str, int c);
int     strcmp(char *s1, char *s2);
int     strncmp(char *s1, char* s2, uint n);

/* bitmap */

#endif
