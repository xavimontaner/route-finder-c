#ifndef PLACES_H
#define PLACES_H

typedef struct Place {
    char id[100];
    char name[200];
    char category[100];
    double lat;
    double lon;
    struct Place* next;
} Place;

Place* load_places(const char* map_name, int* count);
void   free_places(Place* head);
Place* find_place_by_name(Place* head, const char* name);
int    count_places_by_name(Place* head, const char* name);
Place* get_place_by_index(Place* head, const char* name, int n);
//fuzzy search with Levenshtein
Place* find_place_fuzzy(Place* head, const char* name);

#endif