#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "report.h"

void create_district(const char *district){
    struct stat st;

    if(stat(district, &st)== 0){
        printf("District '%s' already exists.\n", district);
        return;
    }

    if(mkdir(district, 0750)<0){
        perror("mkdir Error---failed!!!!!!!");
        return;
    }

    char path[256];
    int fd;

    snprintf(path, sizeof(path), "%s/reports.dat", district);
    fd = open(path, O_CREAT | O_RDWR, 0664);
    if (fd < 0) {
        perror("reports.dat creation failed");
        return;
    }
    close(fd);
    chmod(path, 0664);

    
    snprintf(path, sizeof(path), "%s/district.cfg", district);
    fd = open(path, O_CREAT | O_RDWR, 0640);
    if (fd < 0) {
        perror("district.cfg creation failed");
        return;
    }

    //initialize threshold
    char *default_cfg = "threshold=1\n";
    write(fd, default_cfg, strlen(default_cfg));

    close(fd);
    chmod(path, 0640);

    snprintf(path, sizeof(path), "%s/logged_district", district);
    fd = open(path, O_CREAT | O_RDWR, 0644);
    if (fd<0) {
        perror("log file creation failed");
        return;
    }
    close(fd);
    chmod(path, 0644);

    printf("District '%s' created->it s good.\n", district);
}

void add_report(const char *district, const char *user, const char *role)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    int max_id = 0;
    Report tmp;

    int fd_tmp = open(path, O_RDONLY);
    if (fd_tmp >= 0) {
        while (read(fd_tmp, &tmp, sizeof(Report)) == sizeof(Report)) {
            if (tmp.id > max_id)
                max_id = tmp.id;
        }
        close(fd_tmp);
    }

    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0664);
    if (fd < 0) {
        perror("open reports.dat");
        return;
    }

    Report r;
    memset(&r, 0, sizeof(Report));

    r.id = max_id + 1;

    strncpy(r.inspector, user, NAME_LEN - 1);
    r.inspector[NAME_LEN - 1] = '\0';

    r.latitude = 0.0;
    r.longitude = 0.0;

    strncpy(r.category, "road", CATEGORY_LEN - 1);
    r.category[CATEGORY_LEN - 1] = '\0';

    r.severity = 1;
    r.timestamp = time(NULL);

    strncpy(r.description, "auto-generated report", DESC_LEN - 1);
    r.description[DESC_LEN - 1] = '\0';

   
    ssize_t w = write(fd, &r, sizeof(Report));
    if (w != sizeof(Report)) {
        perror("write failed");
        close(fd);
        return;
    }

    close(fd);

    printf("Report added successfully in %s (ID=%d)\n", district, r.id);
}
// void add_report(const char *district,const char *user,const char *role){
    
    
//     char path[256];
//     snprintf(path, sizeof(path), "%s/reports.dat", district);

//     // chmod(path, 0664);
//     int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0664);
//     if (fd < 0) {
//         perror("open reports.dat");
//         return;
//     }

//     Report r;
//     int max_id = 0;
//     Report tmp;

//     int fd_tmp = open(path, O_RDONLY);
//     if (fd_tmp >= 0) {
//         while (read(fd_tmp, &tmp, sizeof(Report)) == sizeof(Report)) {
//             if (tmp.id > max_id){
//                max_id = tmp.id;
//             }
//         }
//         close(fd_tmp);
//     }

//     r.id = max_id + 1;

    
//     strncpy(r.inspector, user, NAME_LEN);
//     r.inspector[NAME_LEN - 1] = '\0';

//     r.latitude = 0.0;
//     r.longitude = 0.0;

//     strncpy(r.category, "road", CATEGORY_LEN);
//     r.category[CATEGORY_LEN - 1] = '\0';

//     r.severity = 1;

//     r.timestamp = time(NULL);

//     strncpy(r.description, "auto-generated report", DESC_LEN);
//     r.description[DESC_LEN - 1] = '\0';

   
//     ssize_t w = write(fd, &r, sizeof(Report));

//     if (w != sizeof(Report)){
//        perror("write failed");
//     }
//     printf("DEBUG: wrote %ld bytes\n", w);

//     close(fd);
//     printf("Report added successfully in %s (ID=%d)\n", district, r.id);
// }



void list_reports(const char *district){
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    struct stat st;
    if (stat(path, &st) < 0) {
        perror("stat");
        return;
    }

    printf("\n--- FILE INFO ---\n");
    printf("Size: %lld bytes\n", (long long)st.st_size);
    printf("Last modified: %s", ctime(&st.st_mtime));

    char perms[10] = "---------";
    if(st.st_mode & S_IRUSR) perms[0]='r';
    if (st.st_mode & S_IWUSR) perms[1]='w';
    if(st.st_mode & S_IRGRP) perms[3]= 'r';
    if(st.st_mode & S_IWGRP) perms[4]= 'w';
    if(st.st_mode & S_IROTH) perms[6]='r';

    printf("Permissions: %s\n\n", perms);

    int fd = open(path, O_RDONLY);
    if (fd < 0){
        perror("open");
        return;
    }

    Report r;
    printf("--- REPORTS ---\n");
    while (read(fd, &r, sizeof(Report))>0){
        printf("ID:%d | User:%s | Cat:%s | Sev:%d\n",r.id, r.inspector, r.category, r.severity);
    }
    close(fd);
}

void view_report(const char *district, int target_id){
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Report r;
    int found = 0;

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.id == target_id) {
            printf("\n--- REPORT DETAILS ---\n");
            printf("ID: %d\n", r.id);
            printf("Inspector: %s\n", r.inspector);
            printf("Category: %s\n", r.category);
            printf("Severity: %d\n", r.severity);
            printf("Latitude: %.2f\n", r.latitude);
            printf("Longitude: %.2f\n", r.longitude);
            printf("Timestamp: %s", ctime(&r.timestamp));
            printf("Description: %s\n", r.description);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Report ID %d not found in %s\n", target_id, district);
    }
    close(fd);
    
}
