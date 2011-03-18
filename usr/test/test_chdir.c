#include <param.h>
#include <file.h>
#include <unistd.h>

int a=3;

int main(int argc, char **argv) {
    int fd;

    printf("open about.txt: %d\n", open("about.txt", O_RDWR, 0));
    chdir("home");
    printf("cd home\n");
    printf("open about.txt: %d\n", open("about.txt", O_RDWR, 0));
    chdir(".");
    printf(".\n");
    printf("open about.txt: %d\n", open("about.txt", O_RDWR, 0));
    return 0;
}

