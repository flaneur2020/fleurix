#include <param.h>
#include <file.h>
#include <unistd.h>

int a=3;

int strlen(char *str) {
    char* sp;
    for(sp=str; *sp!='\0'; sp++);
    return sp-str;
}

int main(int argc, char **argv) {
    int fd;
    char str[] = "hello, world\n";

    fd = open("/dev/tty0", O_RDONLY, 0);
    if (fork()) {
        while(a) 
            write(fd, "0", 1);
    }
    while(a);
    return 0;
}
