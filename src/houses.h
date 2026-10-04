#ifndef HOUSES_H
#define HOUSES_H

typedef struct House {
    char street_name[150];
    int  house_number;
    double lat;
    double lon;
    struct House* next;
} House;

House* load_houses(const char* map_name, int* count);
void   free_houses(House* head);
void   normalize_name(char* dest, const char* src);
int    levenshtein(const char* s1, const char* s2);
int    find_and_return_coords(House* head, const char* norm_street,int number, double* lat, double* lon);
void   handle_number(House* houses, const char* norm_st, int target_num,double* lat, double* lon);
void   handle_unknown_street(House* houses, const char* norm_input,int target_num, double* lat, double* lon);
#endif