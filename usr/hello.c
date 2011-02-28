#include <param.h>
#include <file.h>
#include <unistd.h>

int i;

int strlen(char *str) {
    char* sp;
    for(sp=str; *sp!='\0'; sp++);
    return sp-str;
}

int main(int argc, char **argv) {
    int fd;
    char str[] = "/bin/hello, world\n";

    write(1, str, sizeof(str));
    for (i=0; i<argc; i++) {
        write(fd, "\n", 1);
        write(fd, argv[i], strlen(argv[i]));
    }
    return 0;
}
