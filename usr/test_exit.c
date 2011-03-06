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
    char str[] = "proc1\n";
    char str2[] = "proc2\n";

    fd = open("/dev/tty0", O_RDONLY, 0);
    if (fork()==0) {
        write(1, str, sizeof(str));
    }
    else {
        write(1, str2, sizeof(str));
    }
    return 1;
}
