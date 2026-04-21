#ifndef FILE_OPS_H
#define FILE_OPS_H
void create_district(const char *district);
void add_report(const char *district,const char *user,const char *role);
void list_reports(const char *district);
void view_report(const char *district, int target_id);
#endif