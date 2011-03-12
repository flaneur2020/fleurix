#include <param.h>
#include <file.h>
#include <unistd.h>

int main(int argc, char **argv){
    uint ret, pid;
    if (fork()==0) {
        exec("/bin/test_exit", NULL);
    }
    while(1) {
        pid = waitpid(0, &ret, 0);
        printf("%d exited (%x)\n", pid, ret);
    }
}
