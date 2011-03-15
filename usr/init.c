#include <param.h>
#include <file.h>
#include <unistd.h>

int main(int argc, char **argv){
    uint ret, pid;
    if (fork()==0) {
        exec("/bin/sh", NULL);
    }
    while(1) {
        pid = wait(&ret);
        //printf("exited - pid %d => %d\n", pid, ret);
    }
}
