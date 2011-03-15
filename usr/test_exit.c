#include <param.h>
#include <file.h>
#include <unistd.h>

int a=3;

int main(int argc, char **argv) {
    int fd;
    int ret;
    char str[] = "proc1\n";
    char str2[] = "proc2\n";
    uint pid;

    fd = open("/dev/tty0", O_RDONLY, 0);
    if (fork()==0) {
        printf("child1\n");
        exec("/bin/hello", NULL);
    }
    if (fork()==0) {
        printf("child2\n");
        _exit(0);
    }
    if (fork()==0){
        int *badmem = (int *)0;
        *badmem = 1;
        printf("so bad~~~\n");
    }
    if (fork()==0) {
    }
    return 1;
}
