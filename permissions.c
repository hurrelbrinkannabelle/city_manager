
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "permissions.h"

int check_permission(const char *file, const char *role, const char *operation){
    struct stat st;

    if (stat(file, &st) == -1) {
        perror("stat");
        return 0;
    }

    mode_t mode = st.st_mode;

    if (strcmp(operation, "read") == 0) {
        if (strcmp(role, "manager") == 0)
            return (mode & S_IRUSR);
        if (strcmp(role, "inspector") == 0)
            return (mode & S_IRGRP);
    }

    if (strcmp(operation, "write") == 0) {
        if (strcmp(role, "manager") == 0)
            return (mode & S_IWUSR);
        if (strcmp(role, "inspector") == 0)
            return (mode & S_IWGRP);
    }

    return 0;
}

void mode_to_string(mode_t mode, char *str) {
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';

    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';

    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';

    str[9] = '\0';
}