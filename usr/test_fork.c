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
    char str[] = "hello, world\n";

    printf("-------\n");
    printf("argv[0]: %s...\n", argv[0]);
    printf("------\n");
    if (fork()==0){
        //while(1) printf("2");
    }
    else if(fork()==0){
        while(1) printf("1");;
    }
    else {
        while(1) printf("3");;
    }
    while(1);
    return 0;
}

