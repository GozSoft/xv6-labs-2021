#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, int argv[]) {
    int p[2];
    pipe(p);
    char *msg = "j";
    
    if(fork() == 0) {   // child
        char buf[2];
        while(!read(p[0], buf, 1)) continue;
        printf("%d: received ping\n", getpid());
        write(p[1], buf, 1);
        exit(0);
    } else {            // parent
        write(p[1], msg, 1);
        wait((int *)0);
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}