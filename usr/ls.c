#include <param.h>
#include <file.h>
#include <stat.h>
#include <unistd.h>
#include <dirent.h>

int ls(char *pathp){
    struct stat sbuf;
    struct dirent dbuf;
    char pathbuf[1024];
    int fd, r;

    if (pathp==NULL)
        strcpy(pathbuf, ".");
    else 
        strncpy(pathbuf, pathp, 1024);
    //
    fd = open(pathbuf, O_RDONLY, 0);
    if (fd<0) {
        return -1;
    }
    fstat(fd, &sbuf);
    if (!S_ISDIR(sbuf.st_mode)){
        return -1;
    }

    while((r=read(fd, &dbuf, sizeof(struct dirent)))>0)
        printf("%s\t", dbuf.d_name);
}

int main(int argc, char **argv){
    int i = 0;

    if (argc==1) 
        ls(NULL);
    else{
        for(i=1; i<argc; i++)
            ls(argv[i]);
    }
    printf("\n");
}
