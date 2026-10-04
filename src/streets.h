#ifndef STREETS_H
#define STREETS_H

typedef struct Street {
    long long from_id;
    double    from_lat, from_lon;
    long long to_id;
    double    to_lat,   to_lon;
    double    length;
    char      name[150];
    struct Street* next;
} Street;

typedef struct Edge {
    Street* street;
    struct Edge* next;
} Edge;

typedef struct Intersection {
    long long  id;
    Edge* connections;
    struct Intersection* next;
} Intersection;

Street*       load_streets(const char* map_name, int* count);
void          free_streets(Street* head);
Street*       find_closest_street(Street* head, double lat, double lon);
void          build_intersection_map(Street* head);
void          free_intersection_map(void);
Intersection* get_intersection(long long id);
void          find_route_bfs(Street* start, Street* end);
void          find_route_bfs_sequential(Street* start, Street* end, Street* all_streets);

#endif