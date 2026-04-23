#ifndef PERMISSIONS_H
#define PERMISSIONS_H
#include <sys/stat.h>

int check_permission(const char *file, const char *role, const char *operation);
void mode_to_string(mode_t mode, char *str);
#endif