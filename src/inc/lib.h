#ifndef LIB_H
#define LIB_H

#ifndef NULL
#define NULL ((void*)0)
#endif

/* prototypes */

/* string */
void*   memcpy(void *dest, void *src, unsigned int count);
void*   memset(void *dest, char val, unsigned int count);
short*  memsetw(short *dest, short val, unsigned int count);
int     strlen(char *str);
char*   strchr(const char *str, int c);
char*   strrchr(const char *str, int c);
int     strcmp(char *s1, char *s2);
int     strncmp(char *s1, char* s2, unsigned int n);

/* bitmap */

#endif
