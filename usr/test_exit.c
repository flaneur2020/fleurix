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
    int ret;
    char str[] = "proc1\n";
    char str2[] = "proc2\n";

    fd = open("/dev/tty0", O_RDONLY, 0);
    if (fork()==0) {
        printf("2");
        printf("3");
        _exit(0);
    }
    else {
        waitpid(0, &ret, 0);
        printf("-- exited :%x\n", ret);
        waitpid(0, &ret, 0);
        printf("-- exited :%x\n", ret);
    }
    return 1;
}
