
#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include "file_ops.h"


int main(int argc, char *argv[]){
    
    char*user=NULL;
    char*role=NULL;
    char*command=NULL;
    char*district=NULL;
    int report_id = -1;
    int value=-1;
    int conditions_start = -1;
    int condition_count = 0;

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
        else if (strcmp(argv[i], "--view") == 0){
           command = "view";
           district = argv[++i];
           report_id = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--remove_report") == 0){
            command = "remove";
            district = argv[++i];
            report_id = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--update_threshold") == 0) {
            command = "update_threshold";
            district = argv[++i];
            value = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--filter") == 0) {
            command = "filter";
            if (i + 1 >= argc) {
                printf("Missing district for filter\n");
                return 1;
            }
            district = argv[++i];
            conditions_start = i + 1;
            condition_count = argc - conditions_start;
            break;
        }
        

    }
    if(command && strcmp(command, "add")==0){
        if(!district || !user || !role) {
            printf("Missing arguments for add command\n");
            return 1;
        }
        if(!district_exists(district)){
            create_district(district);
        }
        printf("Add command parsed successfully for district: %s\n", district);
        add_report(district, user,role);
    
    }
    else if(command && strcmp(command,"list")==0){
        list_reports(district,user,role);
    }
    else if (command && strcmp(command, "view") == 0){
        view_report(district,user,role, report_id);
    }
    else if (command && strcmp(command, "remove") == 0){
        if (!district || report_id < 0 || !role) {
            printf("Missing arguments for remove_report\n");
            return 1;
        }

        if (strcmp(role, "manager") != 0) {
            printf("Error: only manager can remove reports\n");
            return 1;
        }
        remove_report(district, role, report_id);
    }
    else if (command && strcmp(command, "update_threshold") == 0){
        if (strcmp(role, "manager") != 0){
            printf("Error: only manager can update threshold\n");
            return 1;
        }
        update_threshold(district, role, value);
    }
    else if (command && strcmp(command, "filter") == 0){
        if (!district || condition_count <= 0) {
           printf("Missing arguments for filter\n");
           return 1;
        }
        filter_reports(district, &argv[conditions_start], condition_count);
    }
    
    
    return 0;
}
