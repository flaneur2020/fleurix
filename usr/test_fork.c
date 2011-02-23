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
    if (fork()==0) {
        while(1) write(fd, "2", 1);
    }
    else {
        while(1) write(fd, "1", 1);
        //while(1) write(fd, "1", 1);
    }
    while(1);
    return 0;
}
