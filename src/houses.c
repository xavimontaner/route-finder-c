#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "houses.h"

static void expand_name(char* dest, const char* prefix, const char* suffix) {
    size_t prefix_len = strlen(prefix);
    memcpy(dest, prefix, prefix_len);
    size_t i = 0;
    while (suffix[i] != '\0' && prefix_len + i < 149) {
        dest[prefix_len + i] = suffix[i];
        i++;
    }
    dest[prefix_len + i] = '\0';
}

//makes the street names easier to compare
void normalize_name(char* dest, const char* src) {
    char temp[150];
    int j = 0;

    //convert everything to lowercase and remove line jumps
    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] != '\r' && src[i] != '\n') {
            if (j < (int)sizeof(temp) - 1) temp[j++] = (char)tolower((unsigned char)src[i]);
        }
    }
    //quit blank spaces
    while (j > 0 && temp[j - 1] == ' ') {
        j--;
    }
    temp[j] = '\0';

    //expand abbreviations
    if (strncmp(temp, "c. de ", 6) == 0) {
        expand_name(dest, "carrer de ", temp + 6);
    } else if (strncmp(temp, "c. ", 3) == 0) {
        expand_name(dest, "carrer ", temp + 3);
    } else if (strncmp(temp, "av. de ", 7) == 0) {
        expand_name(dest, "avinguda de ", temp + 7);
    } else if (strncmp(temp, "av. ", 4) == 0) {
        expand_name(dest, "avinguda ", temp + 4);
    } else if (strncmp(temp, "avda. ", 6) == 0) {
        expand_name(dest, "avinguda ", temp + 6);
    } else if (strncmp(temp, "pg. ", 4) == 0) {
        expand_name(dest, "passeig ", temp + 4);
    } else if (strncmp(temp, "pl. ", 4) == 0) {
        expand_name(dest, "placa ", temp + 4);
    } else if (strncmp(temp, "ptge. ", 6) == 0) {
        expand_name(dest, "passatge ", temp + 6);
    } else if (strncmp(temp, "rbla. ", 6) == 0) {
        expand_name(dest, "rambla ", temp + 6);
    } else if (strncmp(temp, "ctra. ", 6) == 0) {
        expand_name(dest, "carretera ", temp + 6);
    } else {
        //if no abbreviation found
        strcpy(dest, temp);
    }
}

//calculates similarity between 2 strings
int levenshtein(const char* s1, const char* s2) {
    int len1 = strlen(s1), len2 = strlen(s2);

    //matrix used for distances
    int matrix[len1 + 1][len2 + 1];

    //fill first column
    for (int i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }

    //fill first row
    for (int j = 0; j <= len2; j++) {
        matrix[0][j] = j;
    }

    //calculate distances
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            int m1 = matrix[i - 1][j] + 1;
            int m2 = matrix[i][j - 1] + 1;
            int m3 = matrix[i - 1][j - 1] + cost;
            matrix[i][j] = (m1 < m2 && m1 < m3) ? m1 : (m2 < m3 ? m2 : m3);
        }
    }
    return matrix[len1][len2];
}

//looks for a house and returns its coordinates
int find_and_return_coords(House* head, const char* norm_street, int number, double* lat, double* lon) {
    House* curr = head;

    //go through linked list
    while (curr) {
        char norm_curr[150];
        normalize_name(norm_curr, curr->street_name);

        //same street and same number
        if (strcmp(norm_curr, norm_street) == 0 && curr->house_number == number) {
            *lat = curr->lat;
            *lon = curr->lon;
            return 1;
        }
        curr = curr->next;
    }

    //house not found
    return 0;
}


//handles invalid numbers and asks again
void handle_number(House* houses, const char* norm_st, int target_num, double* lat, double* lon) {
    
    //if house exists directly print coords
    if (find_and_return_coords(houses, norm_st, target_num, lat, lon)) {
        printf("\n    Found at (%f, %f)\n", *lat, *lon);
        return;
    }

    //show available numbers
    printf("\n    Number %d not found on this street. Valid numbers: ", target_num);
    House* curr = houses;
    while (curr) {
        char norm_curr[150];
        normalize_name(norm_curr, curr->street_name);
        if (strcmp(norm_curr, norm_st) == 0)
            printf("[%d] ", curr->house_number);
        curr = curr->next;
    }
    printf("\n    Select a valid number: ");
    int new_num;

    //read new number
    if (scanf("%d", &new_num) == 1) {

        //check again
        if (find_and_return_coords(houses, norm_st, new_num, lat, lon)) {
            printf("\n    Found at (%f, %f)\n", *lat, *lon);
        } else {
            printf("    Invalid selection.\n");
        }
    }
}


//used to store street suggestions
#define MAX_SUGGESTIONS 5

typedef struct {
    char name[150];
    int  dist;
} Suggestion;

//used by qsort to order suggestions
static int cmp_suggestion(const void* a, const void* b) {
    return ((Suggestion*)a)->dist - ((Suggestion*)b)->dist;
}

//shows similar street names when street is not found
void handle_unknown_street(House* houses, const char* norm_input, int target_num, double* lat, double* lon) {

    //dynamic array for unique street names
    int cap = 1024, n = 0;
    char (*names)[150] = malloc(cap * sizeof(*names));
    House* curr = houses;

    //collect all unique names
    while (curr) {
        char norm_curr[150];
        normalize_name(norm_curr, curr->street_name);
        //add if not present
        int found = 0;

        //check if already stored
        for (int i = 0; i < n; i++) {
            if (strcmp(names[i], norm_curr) == 0) { 
                found = 1; break; 
            }
        }

        //save new street
        if (!found) {

            //resize if needed
            if (n == cap) { cap *= 2; names = realloc(names, cap * sizeof(*names)); }
            strcpy(names[n++], norm_curr);
        }
        curr = curr->next;
    }

    //array with best suggestions
    Suggestion sug[MAX_SUGGESTIONS];
    for (int s = 0; s < MAX_SUGGESTIONS; s++) {
        sug[s].dist = 1 << 30;
        sug[s].name[0] = '\0';
    }

    //find closest street names
    for (int i = 0; i < n; i++) {
        int d = levenshtein(norm_input, names[i]);
        if (d < sug[MAX_SUGGESTIONS - 1].dist) {
            sug[MAX_SUGGESTIONS - 1].dist = d;
            strcpy(sug[MAX_SUGGESTIONS - 1].name, names[i]);

            //sort suggestions by distance
            qsort(sug, MAX_SUGGESTIONS, sizeof(Suggestion), cmp_suggestion);
        }
    }
    free(names);

    //print suggestions
    printf("\n    Street not found. Did you mean?\n");
    int valid = 0;
    for (int s = 0; s < MAX_SUGGESTIONS; s++) {
        if (sug[s].name[0]) {
            printf("    [%d] %s\n", s + 1, sug[s].name);
            valid++;
        }
    }
    printf("    Choose (1-%d) or 0 to cancel: ", valid);
    int choice;

    //if user chooses one suggestion
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= valid) {
        handle_number(houses, sug[choice - 1].name, target_num, lat, lon);
    } else {
        printf("    Cancelled.\n");
    }
}

//loads all houses from houses.txt
House* load_houses(const char* map_name, int* count) {
    char path[150];

    //build path to txt file
    if (snprintf(path, sizeof(path), "maps/%s/houses.txt", map_name) >= (int)sizeof(path)) return NULL;
    FILE* f = fopen(path, "r");
    if (!f) return NULL;
    House* head = NULL;
    char line[300];
    *count = 0;

    //read file line by line
    while (fgets(line, sizeof(line), f)) {
        House* h = malloc(sizeof(House));
        if (!h) { free_houses(head); fclose(f); return NULL; }

        //split csv values
        char* tok = strtok(line, ",");
        if (!tok) { free(h); continue; }
        snprintf(h->street_name, sizeof(h->street_name), "%s", tok);
        tok = strtok(NULL, ",");
        if (!tok) { free(h); continue; }
        h->house_number = atoi(tok);
        tok = strtok(NULL, ",");
        if (!tok) { free(h); continue; }
        h->lat = atof(tok);
        tok = strtok(NULL, ",");
        if (!tok) { free(h); continue; }
        h->lon = atof(tok);

        //insert at beginning of linked list
        h->next = head;
        head = h;
        (*count)++;
    }
    fclose(f);
    return head;
}

//free linked list memory
void free_houses(House* head) {
    while (head) {
        House* tmp = head;
        head = head->next;
        free(tmp);
    }
}