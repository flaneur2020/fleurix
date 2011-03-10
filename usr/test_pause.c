#include <param.h>
#include <file.h>
#include <unistd.h>
#include <signal.h>

int a=3;

int main(int argc, char **argv) {
    int fd;
    int ret;
    uint pid;

    if ((pid=fork())==0) {
        pause();
        printf("pause interrupted\n");
        return 0;
    }
    kill(pid, SIGCONT);
    printf("SIGINT has been sent\n");
    return 0;
}
