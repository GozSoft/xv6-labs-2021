#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pi[2];
    pipe(pi);

    int p = 2;
    printf("prime %d\n", p);
    for(int i = p; i <= 31; i++)
        if(i % p) write(pi[1], &i, 1);

    close(pi[1]);
    int rfd = pi[0];

    while(1) {
        int pi2[2];
        pipe(pi2);

        int n;
        read(rfd, &p, 1);
        printf("prime %d\n", p);
        while(read(rfd, &n, 1))
            if(n % p) write(pi2[1], &n, 1);

        close(pi2[1]);
        close(rfd);
        rfd = pi2[0];

        if(p == 31) break;
        if(fork()) break;
    }

    wait(0);
    close(rfd);

    exit(0);
}

// fork()
// p = get a number from left neighbor
// print p
// loop:
//     n = get a number from left neighbor
//     if (p does not divide n)
//         send n to right neighbor