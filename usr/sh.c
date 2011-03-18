#include <param.h>
#include <file.h>
#include <unistd.h>

#define LINEBUFSIZ 1024
#define ARGVBUFSIZ 1024
#define PATHBUFSIZ 1024

char linebuf[LINEBUFSIZ];
char* argvbuf[ARGVBUFSIZ];
char pathbuf[PATHBUFSIZ];

int hwsig(int sign){
    printf("^C\n");
}

int lparse(char *lbuf) {
    char *lp, c;
    int argc, i;

    argc = 0;
    lp = lbuf;
    while((c=*lp++) && lp<&linebuf[LINEBUFSIZ]) {
        if (c == ' ') 
            continue;
        argvbuf[argc] = lp-1; 
        argc++;
        while(*lp++ != ' ');
    }
    for(i=0; i<LINEBUFSIZ; i++){
        if (lbuf[i]==' ') {
            lbuf[i]='\0';
        }
    }
    argvbuf[argc] = NULL;
    return argc;
}

int main(int argc, char **argv){
    int r,fd, i, cnt, ret; 
    struct sigaction sa;

    setpgrp();
    fd = open("/dev/tty0", O_RDWR, 0);
    close(0);
    close(1);
    close(2);
    dup(fd);
    dup(fd);
    dup(fd);
    fcntl(0, F_SETFD, 0); // turn off FD_CLOEXEC
    fcntl(1, F_SETFD, 0);
    fcntl(2, F_SETFD, 0);
    close(fd);
    //
    sa.sa_handler = &hwsig;
    sigaction(SIGINT, &sa, NULL);
    //
    while(1) {
        printf("$ ");
        memset(linebuf, 0, LINEBUFSIZ);
        r = read(0, linebuf, 1024);
        linebuf[r] = 0;
        cnt = lparse(linebuf);
        if (cnt > 0) {
            if (strncmp(argvbuf[0], "cd", 2)==0) {
                chdir(argvbuf[1]);
            }
            else {
                if (fork()==0) {
                    exec(argvbuf[0], &argvbuf[1]);
                    strcpy(pathbuf, "/bin/");
                    strncat(pathbuf, argvbuf[0], PATHBUFSIZ);
                    exec(pathbuf, &argvbuf[1]);
                    exit(1);
                }
                wait(&ret);
            }
        }
    }
}
