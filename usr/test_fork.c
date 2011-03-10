#include <param.h>
#include <file.h>
#include <unistd.h>

int a=3;

int main(int argc, char **argv) {
    int fd;
    char str[] = "hello, world\n";

    if (fork()==0){
        while(1) printf("2");
    }
    else if(fork()){
        while(1) printf("1");;
    }
    else {
        while(1) printf("3");;
    }
    while(1);
    return 0;
}

