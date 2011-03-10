#include <param.h>
#include <file.h>
#include <unistd.h>

int main(int argc, char **argv){
    uint ret;
    if (fork()==0) {
        exec("/bin/hello", NULL);
        return 0;
    }
    while(1) {
        waitpid(0, &ret, 0);
    }
}
