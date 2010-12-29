#include <unistd.h>

_SYS2(int, access, const char*, int);

int open(const char *path, int mode, ...){
}

_SYS1(int, close, int);

_SYS0(int, setup);
_SYS0(int, fork); 
_SYS1(int, exit, int); 
_SYS1(int, nice, int);
_SYS1(int, chdir, char*);
_SYS1(int, setuid, int);
_SYS0(int, debug);
