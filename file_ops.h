#ifndef FILE_OPS_H
#define FILE_OPS_H
void create_district(const char *district);
int district_exists(const char *district);
void add_report(const char *district,const char *user,const char *role);
void list_reports(const char *district, const char *user, const char *role);
void view_report(const char *district, const char *user, const char *role, int target_id);
int remove_report(const char *district, const char *role, int report_id);
int update_threshold(const char *district, const char *role, int value);
void filter_reports(const char *district, char **conditions, int count);
#endif