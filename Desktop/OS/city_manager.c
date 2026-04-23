
#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include "file_ops.h"

// void create_district(const char *district);

int main(int argc, char *argv[]){
    
    char*user=NULL;
    char*role=NULL;
    char*command=NULL;
    char*district=NULL;
    int report_id = -1;

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"--role")==0){
            role=argv[++i];
        }
        else if(strcmp(argv[i],"--user")==0){
            user=argv[++i];
        }
        else if(strcmp(argv[i],"--add")==0){
            command="add";
            district=argv[++i];
        }
        else if(strcmp(argv[i], "--list") == 0){
           command = "list";
           district = argv[++i];
        }
        else if (strcmp(argv[i], "--view") == 0) {
           command = "view";
           district = argv[++i];
           report_id = atoi(argv[++i]);
        }
        

    }
    if(command && strcmp(command, "add")==0){
        if(!district || !user || !role) {
            printf("Missing arguments for add command\n");
            return 1;
        }
        create_district(district);
        printf("Add command parsed successfully for district: %s\n", district);
        add_report(district, user,role);
    
    }
    if(command && strcmp(command,"list")==0){
        list_reports(district);
    }
    if (command && strcmp(command, "view") == 0) {
        view_report(district, report_id);
    }
    
    
    return 0;
}


// city_manager.c   → main + argument parsing
// file_ops.c       → add, list, view, remove
// permissions.c    → stat + permission checks
// filter.c         → filter logic + AI functions
// utils.c          → helpers (mode_to_string, logging, etc.)
// report.h         → struct + constants
// ai_usage.md      → required documentation
