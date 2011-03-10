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
        _exit(0);
    }
    if (fork()==0) {
        printf("child2\n");
        _exit(0);
    }
    while(1){
        pid = waitpid(0, &ret, 0);
        printf("--%d exited :%x\n", pid, ret);
    }
    return 1;
}
