#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *lastname(char *path) {
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    return p;
}

/*
echo > b
mkdir a
echo > a/b
find . b
*/

void find(char *path, char *tar) {
    // printf("Finding in \"%s\" for \"%s\"\n", path, tar);
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            if (!strcmp(lastname(path), tar)) printf("%s\n", path);
            break;
        case T_DIR:
            if (strlen(tar) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("find: tar too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0 || !strcmp(".", de.name) ||
                    !strcmp("..", de.name))
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                find(buf, tar);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: find file...\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) find(".", argv[i]);

    exit(0);
}