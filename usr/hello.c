#include <param.h>
#include <file.h>
#include <unistd.h>

int a = 3;

int main(int argc, char **argv) {
    int fd;
    char str[] = "hello";

    fd = open("/dev/tty0", O_RDONLY, 0);
    write(fd, str, sizeof(str));
    return 0;
}
