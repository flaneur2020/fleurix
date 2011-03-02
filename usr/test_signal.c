#include <param.h>
#include <file.h>
#include <unistd.h>
#include <signal.h>

int a=3;

int strlen(char *str) {
    char* sp;
    for(sp=str; *sp!='\0'; sp++);
    return sp-str;
}

int hwsig(int n){
    char str[] = "handled";
    write(1, str, sizeof(str));
    for(;;);
}

int main(int argc, char **argv) {
    int fd, i, pid;
    char str[] = "hello, world\n";
    struct sigaction sa;

    sa.sa_handler = hwsig;
    if ((pid=fork())==0) {
        sigaction(SIGINT, &sa, NULL);
        while(1) write(1, str, sizeof(str));
    }
    kill(pid, SIGINT);
    return 0;
}
