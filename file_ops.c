#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "report.h"
#include "permissions.h"

void create_district(const char *district){
    struct stat st;

    if (stat(district, &st)==0){
        printf("District '%s' already exists.\n", district);
        return;
    }

    if (mkdir(district, 0750) <0){
        perror("mkdir failed!!!!!!!!!!!!!!!!");
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
    if (chmod(path, 0664) < 0)
        perror("chmod reports.dat failed");

    snprintf(path, sizeof(path), "%s/district.cfg", district);
    fd = open(path, O_CREAT | O_RDWR, 0640);
    if (fd < 0) {
        perror("district.cfg creation failed");
        return;
    }

    char *default_cfg = "threshold=1\n";
    if (write(fd, default_cfg, strlen(default_cfg)) < 0)
        perror("write cfg failed");

    close(fd);
    if (chmod(path, 0640) < 0)
        perror("chmod district.cfg failed");
    snprintf(path, sizeof(path), "%s/logged_district", district);
    fd = open(path, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        perror("log file creation failed");
        return;
    }
    close(fd);
    if (chmod(path, 0644) < 0)
        perror("chmod logged_district failed");

    printf("District '%s' created successfully.\n", district);

    // active_reports-<district_id> 
    char linkname[256];
    char target[256];

    snprintf(linkname, sizeof(linkname), "active_reports-%s", district);
    snprintf(target, sizeof(target), "%s/reports.dat", district);
    unlink(linkname);


    if (symlink(target, linkname) < 0) {
       perror("symlink failed");
    } else {
       printf("Symlink created: %s -> %s\n", linkname, target);
    }

    // check for dangling link
    struct stat link_st, target_st;

    if (lstat(linkname, &link_st) == 0 && S_ISLNK(link_st.st_mode)) {
       if (stat(linkname, &target_st) < 0) {
          printf("WARNING: dangling symlink: %s\n", linkname);
        }
    }
}
int district_exists(const char *district) {
    struct stat st;
    return (stat(district, &st) == 0);
}
void write_log(const char *district, const char *role, const char *user, const char *action) {
    char path[256];
    snprintf(path, sizeof(path), "%s/logged_district", district);

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("log open failed");
        return;
    }

    time_t now = time(NULL);

    char buffer[512];
    int len = snprintf(buffer, sizeof(buffer),
        "[%ld] role=%s user=%s action=%s\n",
        now, role, user, action
    );

    write(fd, buffer, len);
    close(fd);
}


void notify_monitor(const char *district, const char *role, const char *user){//phase 2 addition 
    char pid_path[256];
    snprintf(pid_path, sizeof(pid_path), ".monitor_pid");

    int fd = open(pid_path, O_RDONLY);
    if (fd < 0) {
        write_log(district, role, user,
            "Monitor NOT notified (missing .monitor_pid)");
        return;
    }

    char buf[64];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    close(fd);

    if (n <= 0) {
        write_log(district, role, user,
            "Monitor NOT notified (empty .monitor_pid)");
        return;
    }

    buf[n] = '\0';

    pid_t monitor_pid = atoi(buf);
    if (monitor_pid <= 0) {
        write_log(district, role, user,
            "Monitor NOT notified (invalid PID)");
        return;
    }

    if (kill(monitor_pid, SIGUSR1) == -1) {
        char msg[128];
        snprintf(msg, sizeof(msg),
            "Monitor NOT notified (kill failed: %s)", strerror(errno));

        write_log(district, role, user, msg);
        return;
    }

    write_log(district, role, user,
        "Monitor notified successfully (SIGUSR1 sent)");
}


void add_report(const char *district, const char *user, const char *role){
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    if (!check_permission(path, role, "write")) {
        printf("ERROR: %s does not have WRITE access to reports.dat\n", role);
        return;
    }

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

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("open reports.dat");
        return;
    }

    Report r;
    memset(&r, 0, sizeof(Report));

    r.id = max_id + 1;
    strncpy(r.inspector, user, NAME_LEN - 1);
    r.inspector[NAME_LEN - 1] = '\0';
    printf("Latitude: ");
    scanf("%f", &r.latitude);
    printf("Longitude: ");
    scanf("%f", &r.longitude);
    do{
        printf("Category (road/lighting/flooding/other): ");
        scanf("%31s", r.category);
    }while (strcmp(r.category, "road") != 0 &&  strcmp(r.category, "lighting") != 0 &&  strcmp(r.category, "flooding") != 0 && strcmp(r.category, "other") != 0);
    do{
      printf("Severity Level (1=minor, 2=moderate, 3=critical): ");
      scanf("%d", &r.severity);
    }while(r.severity <1 || r.severity>3);
    printf("Descriptin: ");
    scanf(" %127[^\n]", r.description); 
    r.timestamp = time(NULL);
   

    if (write(fd, &r, sizeof(Report)) != sizeof(Report)) {
        perror("write failed");
        close(fd);
        return;
    }

    close(fd);

    printf("Report added successfully in %s (ID=%d)\n", district, r.id);
    write_log(district, role, user, "add_report");
    notify_monitor(district, role, user);//added for phase 2
}

void list_reports(const char *district, const char *user, const char *role){
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    if (!check_permission(path, role, "read")) {
        printf("ERROR: %s does not have READ access to reports.dat\n", role);
        return;
    }

    struct stat st;
    if (stat(path, &st)<0) {
        perror("stat");
        return;
    }

    printf("\n--- FILE INFO ---\n");
    printf("Size: %lld bytes\n", (long long)st.st_size);
    printf("Last modified: %s", ctime(&st.st_mtime));

   
    char perms[10];
    mode_to_string(st.st_mode, perms);
    printf("Permissions: %s\n\n", perms);

   
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Report r;
    printf("--- REPORTS ---\n");
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        printf("ID:%d | User:%s | Cat:%s | Sev:%d\n",r.id, r.inspector, r.category, r.severity);
    }

    close(fd);
    write_log(district, role, user, "list_reports");
}


void view_report(const char *district, const char *user, const char *role, int target_id){
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);
    if (!check_permission(path, role, "read")) {
        printf("ERROR: %s does not have READ access to reports.dat\n", role);
        return;
    }

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
    write_log(district, role, user, "view_report");
}


int remove_report(const char *district, const char *role, int report_id){
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("stat");
        return -1;
    }
    if (strcmp(role, "manager") != 0) {
        printf("ERROR: only manager can remove reports\n");
        return -1;
    }
    if (!check_permission(path, "manager", "write")) {
        printf("ERROR: manager has no write permission on reports.dat\n");
        return -1;
    }
    int fd = open(path, O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    Report current;
    off_t pos = 0;
    int found = 0;

    while (read(fd, &current, sizeof(Report)) == sizeof(Report)) {
        if (current.id == report_id) {
            found = 1;
            break;
        }
        pos += sizeof(Report);
    }

    if (!found) {
        printf("Report not found\n");
        close(fd);
        return -1;
    }

    //shift left
    Report next;
    off_t read_pos=pos+sizeof(Report);

    while (lseek(fd, read_pos, SEEK_SET) >= 0 && read(fd, &next, sizeof(Report)) == sizeof(Report)) {

        lseek(fd, pos, SEEK_SET);
        write(fd, &next, sizeof(Report));
        pos += sizeof(Report);
        read_pos += sizeof(Report);
    }
    ftruncate(fd, pos);
    close(fd);
    printf("Report %d removed successfully\n", report_id);
    write_log(district, role, "system", "remove_report");
    return 0;
}


//#####################################################################################
int update_threshold(const char *district, const char *role, int value){

    if (strcmp(role, "manager") != 0) {
        printf("ERROR: only manager can update threshold\n");
        return -1;
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/district.cfg", district);
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("stat");
        return -1;
    }
    if ((st.st_mode & 0777) != 0640) {
        printf("SECURITY ERROR: district.cfg permissions modified!\n");
        printf("Expected: 0640, Found: %o\n", st.st_mode & 0777);
        return -1;
    }

    int fd = open(path, O_WRONLY | O_TRUNC);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "threshold=%d\n", value);
    if (write(fd, buffer, len) != len) {
        perror("write");
        close(fd);
        return -1;
    }
    close(fd);
    printf("Threshold updated to %d in %s\n", value, district);
    return 0;
}


int parse_condition(const char *input, char *field, char *op, char *value){
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *f = strtok(buffer, ":");
    char *o = strtok(NULL, ":");
    char *v = strtok(NULL, ":");

    if (!f || !o || !v)
        return 0;

    strcpy(field, f);
    strcpy(op, o);
    strcpy(value, v);

    return 1;
}

int match_condition(Report *r, const char *field, const char *op, const char *value){
    if (strcmp(field, "severity") == 0) {
        int v = atoi(value);

        if (strcmp(op, "==") == 0) return r->severity == v;
        if (strcmp(op, "!=") == 0) return r->severity != v;
        if (strcmp(op, ">")  == 0) return r->severity > v;
        if (strcmp(op, "<")  == 0) return r->severity < v;
        if (strcmp(op, ">=") == 0) return r->severity >= v;
        if (strcmp(op, "<=") == 0) return r->severity <= v;
    }

    if (strcmp(field, "timestamp") == 0) {
        long v = atol(value);

        if (strcmp(op, "==") == 0) return r->timestamp == v;
        if (strcmp(op, "!=") == 0) return r->timestamp != v;
        if (strcmp(op, ">")  == 0) return r->timestamp > v;
        if (strcmp(op, "<")  == 0) return r->timestamp < v;
        if (strcmp(op, ">=") == 0) return r->timestamp >= v;
        if (strcmp(op, "<=") == 0) return r->timestamp <= v;
    }

    if (strcmp(field, "category") == 0) {
        if (strcmp(op, "==") == 0) return strcmp(r->category, value) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(r->category, value) != 0;
    }

    if (strcmp(field, "inspector") == 0) {
        if (strcmp(op, "==") == 0) return strcmp(r->inspector, value) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(r->inspector, value) != 0;
    }

    return 0;
}

void filter_reports(const char *district, char **conditions, int count){
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Report r;
    char field[64], op[8], value[64];

    printf("--- FILTER RESULTS ---\n");

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {

        int match_all = 1;

        for (int i = 0; i < count; i++) {

            if (!parse_condition(conditions[i], field, op, value)) {
                printf("Invalid condition: %s\n", conditions[i]);
                match_all = 0;
                break;
            }

            if (!match_condition(&r, field, op, value)) {
                match_all = 0;
                break;
            }
        }

        if (match_all) {
            printf("ID:%d | User:%s | Cat:%s | Sev:%d | Time:%ld\n", r.id, r.inspector, r.category, r.severity, r.timestamp);
        }
    }

    close(fd);
}
//should be removed in the parent process
//only manager should be allowed to do this
//use fork() to create  a child process that is a copy of the parent process(pid=fork()) if pid=0 =>child if pid>0 =>
//put what the child does in if(pid==0){} and what parent whant to do in if(pid>0){}
//wait() -> after parent reads termination state then child will be read and cleared by kernel!!! then call the unlink()
//
#include <ctype.h>

int is_safe_id(const char *id){
    if (id==NULL || id[0]=='\0')
        return 0;
    if (strstr(id, "..") != NULL)
        return 0;
    for (int i = 0; id[i] != '\0'; i++) {
        if (!(isalnum(id[i]) || id[i] == '_' || id[i] == '-')) {
            return 0;
        }
    }
    return 1;
}

void remove_district(const char *district, const char *role, const char *user) {

    if (strcmp(role, "manager")!= 0){
        printf("ERROR: only manager can remove a district\n");
        return;
    }
    if (!is_safe_id(district)){
        printf("Invalid district id\n");
        return;
    }

    char dir_path[256];
    char link_path[256];

    snprintf(dir_path, sizeof(dir_path), "%s", district);
    struct stat st;
    if (stat(dir_path, &st) <0){
        printf("ERROR: district '%s' does not exist\n", district);
        return;
    }
    snprintf(link_path, sizeof(link_path), "active_reports-%s", district);
    write_log(district, role, user, "remove_district");
    pid_t pid = fork();

    if(pid<0){
        perror("fork failed");
        return;
    }

    if (pid ==0){
        //CHILD:execute rm -rf
        execlp("rm", "rm", "-rf", dir_path, NULL);
        perror("exec failed");
        _exit(1);
    }

    //Parent waits
    int status;
    if(waitpid(pid,&status,0)==-1){
        perror("waitpid failed");
        return;
    }

    //Check result
    if(WIFEXITED(status) && WEXITSTATUS(status)== 0){
        // remove symlink
        unlink(link_path); 
        write_log(district, role, user, "remove_district");//??????(oricum se sterge tot distructul deic nu ore aare rost sa scriu in logged_distict nimic(se sterege si el))
        printf("District '%s' removed successfully\n", district);
    } 
    else{
        printf("Failed to remove district '%s'\n", district);
    }
}