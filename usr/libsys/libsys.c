#include <param.h>
#include <unistd.h>

int exit(int ret){
    return _exit(ret);
}
