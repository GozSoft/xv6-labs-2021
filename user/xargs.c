#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char* argv[]) {
    char buf[512];
    char* nargv[MAXARG];
    for (int i = 1; i < argc; i++) nargv[i - 1] = argv[i];

    while (1) {
        int ind = -1;
        int t;
        do {
            ind++;
            t = read(0, &buf[ind], 1);
        } while (t && buf[ind] != '\n');

        if (t == 0 && ind == 0) break;

        buf[ind] = '\0';
        nargv[argc - 1] = buf;

        if (fork() == 0) 
            exec(nargv[0], nargv);
        else 
            wait(0);
    }

    exit(0);
}