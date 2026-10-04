#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "places.h"
#include "houses.h"  

#define MAX_SUGGESTIONS 5

// converts text to lowercase and removes trailing spaces or newlines
static void normalize_place_name(char* dest, const char* src) {
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] != '\r' && src[i] != '\n') {
            if (j < 199) dest[j++] = (char)tolower((unsigned char)src[i]);
        }
    }
    while (j > 0 && dest[j - 1] == ' ') j--;
    dest[j] = '\0';
}

// reads places from a text file and loads them into a linked list
Place* load_places(const char* map_name, int* count) {
    char path[150];
    if (snprintf(path, sizeof(path), "maps/%s/places.txt", map_name) >= (int)sizeof(path)) return NULL;
    FILE* f = fopen(path, "r");
    if (f == NULL){
    return NULL;
    }
    Place* head = NULL;
    char line[500];
    *count = 0;
    while (fgets(line, sizeof(line), f)) {
        Place* p = malloc(sizeof(Place));
        if (!p) { free_places(head); fclose(f); return NULL; }
        char* tok = strtok(line, ",");
        if (!tok) { free(p); continue; }
        snprintf(p->id, sizeof(p->id), "%s", tok);
        tok = strtok(NULL, ",");
        if (!tok) { free(p); continue; }
        snprintf(p->name, sizeof(p->name), "%s", tok);
        tok = strtok(NULL, ",");
        if (!tok) { free(p); continue; }
        snprintf(p->category, sizeof(p->category), "%s", tok);
        tok = strtok(NULL, ",");
        if (!tok) { free(p); continue; }
        p->lat = atof(tok);
        tok = strtok(NULL, ",");
        if (!tok) { free(p); continue; }
        p->lon = atof(tok);
        p->next = head;
        head = p;
        (*count)++;
    }
    fclose(f);
    return head;
}

// traverses the linked list to free memory allocated for each place
void free_places(Place* head) {
    while (head) {
        Place* tmp = head;
        head = head->next;
        free(tmp);
    }
}

// loops through the list to find a place with a matching normalized name
Place* find_place_by_name(Place* head, const char* name) {
    char norm_search[200];
    normalize_place_name(norm_search, name);
    Place* curr = head;
    while (curr) {
        char norm_curr[200];
        normalize_place_name(norm_curr, curr->name);
       if ( strcmp ( norm_curr , norm_search ) == 0 ){
        return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// counts how many places match the given search name
int count_places_by_name(Place* head, const char* name) {
    char norm_search[200];
    normalize_place_name(norm_search, name);
    int matches = 0;
    Place* curr = head;
    while (curr) {
        char norm_curr[200];
        normalize_place_name(norm_curr, curr->name);
       if ( strcmp ( norm_curr , norm_search ) == 0 ){
        matches++;
        }
        curr = curr->next;
    }
    return matches;
}

// returns the n-th matching place found in the list
Place* get_place_by_index(Place* head, const char* name, int n) {
    char norm_search[200];
    normalize_place_name(norm_search, name);
    int current_match = 0;
    Place* curr = head;
    while (curr) {
        char norm_curr[200];
        normalize_place_name(norm_curr, curr->name);
        if (strcmp(norm_curr, norm_search) == 0) {
            if (current_match == n) return curr;
            current_match++;
        }
        curr = curr->next;
    }
    return NULL;
}

// uses levenshtein distance to suggest closest matches when exact search fails
Place* find_place_fuzzy(Place* head, const char* name) {
    //try exact match first
    Place* exact = find_place_by_name(head, name);
   if (exact != NULL) {
    return exact;
    }

    char norm_search[200];
    normalize_place_name(norm_search, name);

    typedef struct { Place* place; int dist; } Sug;
    Sug sug[MAX_SUGGESTIONS];
    for (int i = 0; i < MAX_SUGGESTIONS; i++) {
        sug[i].place = NULL;
        sug[i].dist  = 1 << 30;
    }

    Place* curr = head;
    while (curr) {
        char norm_curr[200];
        normalize_place_name(norm_curr, curr->name);
        int d = levenshtein(norm_search, norm_curr);
        if (d < sug[MAX_SUGGESTIONS - 1].dist) {
            sug[MAX_SUGGESTIONS - 1].dist  = d;
            sug[MAX_SUGGESTIONS - 1].place = curr;
            //bubble-sort to keep ascending order
            for (int i = MAX_SUGGESTIONS - 1; i > 0; i--) {
                if (sug[i].dist < sug[i - 1].dist) {
                    Sug tmp = sug[i]; sug[i] = sug[i - 1]; sug[i - 1] = tmp;
                } else break;
            }
        }
        curr = curr->next;
    }

    printf("\n    Place not found. Did you mean?\n");
    int valid = 0;
    for (int i = 0; i < MAX_SUGGESTIONS; i++) {
        if (sug[i].place) {
            printf("    [%d] %s\n", i + 1, sug[i].place->name);
            valid++;
        }
    }
    printf("    Choose (1-%d) or 0 to cancel: ", valid);
    int choice;
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= valid) {
        return sug[choice - 1].place;
    }
    return NULL;
}