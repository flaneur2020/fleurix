#include <param.h>
#include <file.h>
#include <unistd.h>

int a=3;

int main(int argc, char **argv) {
    int fd;
    int ret;
    char str[] = "proc1\n";
    char str2[] = "proc2\n";

    fd = open("/dev/tty0", O_RDONLY, 0);
    if (fork()==0) {
        printf("child1\n");
        _exit(0);
    }
    if (fork()==0) {
        printf("child2\n");
        _exit(1);
    }
    waitpid(0, &ret, 0);
    printf("-- exited :%x\n", ret);
    waitpid(0, &ret, 0);
    printf("-- exited :%x\n", ret);
    return 1;
}
