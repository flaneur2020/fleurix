#include <param.h>
#include <file.h>
#include <unistd.h>

#define LINEBUFSIZ 1024
#define ARGVBUFSIZ 1024
#define PATHBUFSIZ 1024

char linebuf[LINEBUFSIZ];
char* argvbuf[ARGVBUFSIZ];
char pathbuf[PATHBUFSIZ];

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
    int r, i, cnt, ret; 

    while(1) {
        printf("$ ");
        memset(linebuf, 0, LINEBUFSIZ);
        r = read(0, linebuf, 1024);
        linebuf[r] = 0;
        cnt = lparse(linebuf);
        if (cnt > 0) {
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
