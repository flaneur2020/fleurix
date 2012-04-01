#include <param.h>
#include <file.h>
#include <stat.h>
#include <unistd.h>
#include <dirent.h>

int cat(char *pathp){
    int fd;
    char buf[32];
    int n;

    fd = open(pathp, O_RDONLY, 0);
    if (fd < 0) 
        return -1;

    while((n = read(fd, buf, 32)) > 0){
        write(0, buf, n);
    }
    close(fd);
}



int main(int argc, char **argv){
    int i = 0;

    if (argc < 1) {
        return -1;
    }

    for(i=1; i<argc; i++) {
        printf("i: %d\n", i);
        cat(argv[i]);
    }

    printf("\n");
    return 0;
}
