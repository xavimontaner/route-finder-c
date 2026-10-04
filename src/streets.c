#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "streets.h"

#define HASH_SIZE 10000
#define PI 3.14159265358979323846

Intersection* intersection_map[HASH_SIZE];

//calculates the distance between two coordinates in meters
static double haversine(double lat1, double lon1, double lat2, double lon2) {
    double R = 6371000.0;
    double dLat = (lat2 - lat1) * PI / 180.0;
    double dLon = (lon2 - lon1) * PI / 180.0;

    //haversine formula   
    double a = sin(dLat / 2) * sin(dLat / 2)+ cos(lat1 * PI / 180.0) * cos(lat2 * PI / 180.0) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

//loads all street segments from streets.txt
Street* load_streets(const char* map_name, int* count) {
    char path[150];

    //create the path to the file
    if (snprintf(path, sizeof(path), "maps/%s/streets.txt", map_name) >= (int)sizeof(path)) return NULL;
    FILE* f = fopen(path, "r");
    if (!f) 
    return NULL;
    Street* head = NULL;
    char line[500];
    *count = 0;
    //read file line by line
    while (fgets(line, sizeof(line), f)) {
        Street* s = malloc(sizeof(Street));
        if (!s) { free_streets(head); fclose(f); return NULL; }

        //read the values of one street segment
        if (sscanf(line, "%lld,%lf,%lf,%lld,%lf,%lf,%lf,%149[^\r\n]", &s->from_id, &s->from_lat, &s->from_lon, &s->to_id, &s->to_lat, &s->to_lon, &s->length, s->name) != 8) {
            free(s);
            continue;
        }

        //insert at the start of the linked list        
        s->next = head;
        head = s;
        (*count)++;
    }
    fclose(f);
    return head;
}

//free the linked list of streets
void free_streets(Street* head) {
    while (head) {
        Street* tmp = head;
        head = head->next;
        free(tmp);
    }
}

//finds the street segment closest to the coordinate given
Street* find_closest_street(Street* head, double lat, double lon) {
    Street* curr = head;
    Street* best = NULL;
    double  min_dist = 1e18;

    //check every street segment
    while (curr) {
        double mid_lat = (curr->from_lat + curr->to_lat) / 2.0;
        double mid_lon = (curr->from_lon + curr->to_lon) / 2.0;

        //distance from input coordinate to segment midpoint        
        double d = haversine(lat, lon, mid_lat, mid_lon);
        if (d < min_dist) {
            min_dist = d;
            best = curr;
        }
        curr = curr->next;
    }
    return best;
}

//builds the hash map of intersections
void build_intersection_map(Street* head) {
    free_intersection_map();
    Street* curr = head;
    while (curr) {

        //hash using the from intersection id
        int h = abs((int)(curr->from_id % HASH_SIZE));
        Intersection* inter = intersection_map[h];

        //look if the intersection already exists in this bucket
        while (inter && inter->id != curr->from_id)
            inter = inter->next;

        //if it does not exist, create it
        if (!inter) {
            inter = malloc(sizeof(Intersection));
            if (!inter) return;
            inter->id = curr->from_id;
            inter->connections = NULL;
            inter->next = intersection_map[h];
            intersection_map[h] = inter;
        }

        //add this street as a connection from this intersection
        Edge* e = malloc(sizeof(Edge));
        if (!e) return;
        e->street = curr;
        e->next = inter->connections;
        inter->connections = e;
        curr = curr->next;
    }
}

void free_intersection_map(void) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Intersection* inter = intersection_map[i];
        while (inter) {
            Intersection* next_inter = inter->next;
            Edge* edge = inter->connections;
            while (edge) {
                Edge* next_edge = edge->next;
                free(edge);
                edge = next_edge;
            }
            free(inter);
            inter = next_inter;
        }
        intersection_map[i] = NULL;
    }
}

//gets one intersection from the hash map by id
Intersection* get_intersection(long long id) {
    int h = abs((int)(id % HASH_SIZE));
    Intersection* inter = intersection_map[h];

   //search inside the bucket
    while (inter){
    if (inter->id == id) 
    return inter;
    inter = inter->next;
}
return NULL;
}