#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "streets.h"

#define PI 3.14159265358979323846



typedef struct PathNode {
    Street*          street;
    struct PathNode* parent;
    double           cost;
} PathNode;

//bearing and turn direction
static double bearing(double lat1, double lon1, double lat2, double lon2) {
    double dLon = (lon2 - lon1) * PI / 180.0;
    lat1 *= PI / 180.0;
    lat2 *= PI / 180.0;

    //calculate direction between two coordenates
    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    return atan2(y, x) * 180.0 / PI;
}

static const char* turn_direction(Street* from, Street* to) {
    double b1 = bearing(from->from_lat, from->from_lon,
                        from->to_lat,   from->to_lon);
    double b2 = bearing(to->from_lat,   to->from_lon,
                        to->to_lat,     to->to_lon);
    double diff = b2 - b1;

    //keep angle between -180 and 180
    while (diff >  180.0) {
        diff -= 360.0;
    }
    while (diff < -180.0) {
        diff += 360.0;
    }
    return (diff >= 0.0) ? "Turn right to" : "Turn left to";
}

//count how many nodes are in the path
static int path_length(PathNode* node) {
    int len = 0;
    while (node) { 
        len++; node = node->parent; 
    }
    return len;
}

//print route grouping consecutive segments of the same street
static void print_route(PathNode* node) {
    if (!node) {
        return;
    }

    //we convert linked list path into array
    int len = path_length(node);
    PathNode** arr = calloc((size_t)len, sizeof(PathNode*));
    if (!arr) {
        printf("  Out of memory.\n");
        return;
    }
    PathNode* cur = node;
    for (int i = len - 1; i >= 0; i--) {
        arr[i] = cur;
        cur = cur->parent;
    }

    printf("  Start at %s\n", arr[0]->street->name);

    int i = 1;
    while (i < len) {
        const char* cur_name = arr[i]->street->name;
        double total = 0.0;
        int j = i;

        //group consecutive streets with same name
        while (j < len && strcmp(arr[j]->street->name, cur_name) == 0) {
            total += arr[j]->street->length;
            j++;
        }
        const char* dir = turn_direction(arr[i-1]->street, arr[i]->street);
        printf("  %s %s and continue for %.0fm\n", dir, cur_name, total);
        i = j;
    }

    free(arr);
}



//visited set
#define VISITED_SIZE 131072
static Street* visited_set[VISITED_SIZE];

//the visited hash table is reseted
static void visited_clear(void) {
    memset(visited_set, 0, sizeof(visited_set));
}

//we check if street was already visited
static int visited_contains(Street* s) {
    unsigned int h = (unsigned int)((size_t)s % VISITED_SIZE);
    unsigned int i = h;
    while (visited_set[i]) {
        if (visited_set[i] == s) {
            return 1;
        }
        i = (i + 1) % VISITED_SIZE;
        if (i == h) {
            break;
        }
    }
    return 0;
}

//add new visited street
static void visited_add(Street* s) {
    unsigned int h = (unsigned int)((size_t)s % VISITED_SIZE);
    unsigned int i = h;
    while (visited_set[i] && visited_set[i] != s) {
        i = (i + 1) % VISITED_SIZE;
        if (i == h) {
            return;
        }
    }
    visited_set[i] = s;
}

//BFS

void find_route_bfs(Street* start, Street* end) {
    if (!start || !end) { 
        printf("  Invalid start or end.\n"); return; 
    }
    visited_clear();

    int capacity = 65536;

    //queue for bfs traversal
    PathNode** queue = malloc(capacity * sizeof(PathNode*));
    if (!queue) { 
        printf("  Out of memory.\n"); return; 
    }
    int head_idx = 0, tail_idx = 0;
    //we save allocated nodes to free later
    PathNode** allocated = malloc(capacity * sizeof(PathNode*));
    if (!allocated) { free(queue); printf("  Out of memory.\n"); return; }
    int allocated_count = 0;

    PathNode* root = malloc(sizeof(PathNode));
    if (!root) { free(allocated); free(queue); printf("  Out of memory.\n"); return; }
    allocated[allocated_count++] = root;
    root->street = start; 
    root->parent = NULL; 
    root->cost = 0.0;
    queue[tail_idx++] = root;
    visited_add(start);

    //this is the main bfs loop
    while (head_idx < tail_idx) {
        PathNode* curr = queue[head_idx++];

        //destination found
        if (curr->street == end) {
            print_route(curr);
            printf("  You have arrived to %s\n", end->name);
            //we free all allocated pathnodes
            for (int i = 0; i < allocated_count; i++) {
                free(allocated[i]);
            }

            free(allocated);
            free(queue);
            return;
        }

        Intersection* inter = get_intersection(curr->street->to_id);
        if (inter) {
            Edge* e = inter->connections;
            while (e) {
                //it skips the streets already visited
                if (!visited_contains(e->street)) {
                    visited_add(e->street);
                    if (tail_idx == capacity) {
                        int new_capacity = capacity * 2;
                        PathNode** new_queue = realloc(queue, new_capacity * sizeof(PathNode*));
                        if (!new_queue) { printf("  Out of memory.\n"); goto cleanup_bfs; }
                        queue = new_queue;
                        PathNode** new_allocated = realloc(allocated, new_capacity * sizeof(PathNode*));
                        if (!new_allocated) { printf("  Out of memory.\n"); goto cleanup_bfs; }
                        allocated = new_allocated;
                        capacity = new_capacity;
                    }
                    PathNode* next = malloc(sizeof(PathNode));
                    if (!next) { printf("  Out of memory.\n"); goto cleanup_bfs; }
                    allocated[allocated_count++] = next;
                    next->street = e->street;
                    next->parent = curr;
                    next->cost   = 0.0;
                    queue[tail_idx++] = next;
                }
                e = e->next;
            }
        }
    }

    //there is no possible route
    printf("  No route found (BFS).\n");
cleanup_bfs:
    for (int i = 0; i < allocated_count; i++) {
        free(allocated[i]);
    }

    free(allocated);
    free(queue);
}

void find_route_bfs_sequential(Street* start, Street* end, Street* all_streets) {
    //we check that all inputs are valid
    if (!start || !end || !all_streets) {
        printf("  Invalid start or end.\n"); return;
    }
    //we reset the visited set before starting
    visited_clear();

    int capacity = 65536;
    PathNode** queue = malloc(capacity * sizeof(PathNode*));
    if (!queue) { 
        printf("  Out of memory.\n"); return; 
    }
    int head_idx = 0, tail_idx = 0;

    //we allocate array to track all nodes for later cleanup
    PathNode** allocated = malloc(capacity * sizeof(PathNode*));
    if (!allocated) { free(queue); printf("  Out of memory.\n"); return; }
    int allocated_count = 0;

    //create the root node with the start street
    PathNode* root = malloc(sizeof(PathNode));
    if (!root) { free(allocated); free(queue); printf("  Out of memory.\n"); return; }
    allocated[allocated_count++] = root;
    root->street = start;
    root->parent = NULL;
    root->cost   = 0.0;
    queue[tail_idx++] = root;
    visited_add(start);


    //main bfs loop
    while (head_idx < tail_idx) {
        PathNode* curr = queue[head_idx++];

        if (curr->street == end) {
            print_route(curr);
            printf("  You have arrived to %s\n", end->name);
            for (int i = 0; i < allocated_count; i++) {
                free(allocated[i]);
            }
            free(allocated); 
            free(queue);
            return;
        }

        //scan the full street list to find connected streets
        Street* s = all_streets;
        while (s) {
            if (s->from_id == curr->street->to_id) {
                if (!visited_contains(s)) {
                    visited_add(s);
                    if (tail_idx == capacity) {
                        int new_capacity = capacity * 2;
                        PathNode** new_queue = realloc(queue, new_capacity * sizeof(PathNode*));
                        if (!new_queue) { printf("  Out of memory.\n"); goto cleanup_sequential; }
                        queue = new_queue;
                        PathNode** new_allocated = realloc(allocated, new_capacity * sizeof(PathNode*));
                        if (!new_allocated) { printf("  Out of memory.\n"); goto cleanup_sequential; }
                        allocated = new_allocated;
                        capacity = new_capacity;
                    }
                    //create a new path node and add it to the queue
                    PathNode* next = malloc(sizeof(PathNode));
                    if (!next) { printf("  Out of memory.\n"); goto cleanup_sequential; }
                    allocated[allocated_count++] = next;
                    next->street = s;
                    next->parent = curr;
                    next->cost   = 0.0;
                    queue[tail_idx++] = next;
                }
            }
            s = s->next;
        }
    }

    printf("  No route found (BFS sequential).\n");
cleanup_sequential:
    for (int i = 0; i < allocated_count; i++) {
        free(allocated[i]);
    }
    free(allocated); 
    free(queue);
}