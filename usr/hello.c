#include <param.h>
#include <file.h>
#include <unistd.h>

int a = 3;

int strlen(char *str) {
    char* sp;
    for(sp=str; *sp!='\0'; sp++);
    return sp-str;
}

int main(int argc, char **argv) {
    int fd, i;
    char str[] = "hello, world\n";

    fd = open("/dev/tty0", O_RDONLY, 0);
    for (i=0; i<argc;i++) {
        write(fd, "\n", 1);
        write(fd, argv[i], strlen(argv[i]));
    }
    return 0;
}
