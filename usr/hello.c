#include <param.h>
#include <file.h>
#include <unistd.h>

int i;

int main(int argc, char **argv) {
    int fd;

    printf("hello~\n");
    for (i=0; i<argc; i++) {
        write(fd, "\n", 1);
        write(fd, argv[i], strlen(argv[i]));
    }
    return 0;
}
