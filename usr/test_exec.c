#include <param.h>
#include <file.h>
#include <unistd.h>

int a=3;

int main(int argc, char **argv) {
    int fd;
    char str[] = "hello, world\n";

    if (fork()==0) {
        write(1, "children\n", 10);
        exec("/bin/hello", NULL);
    }
    write(1, "parent proc\n", 13);
    return 0;
}
