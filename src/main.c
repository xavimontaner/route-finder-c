#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "houses.h"
#include "places.h"
#include "streets.h"
#include <time.h>

//asks the user where they are or where they want to go
static void get_location(House* houses, Place* places, double* lat, double* lon) {
    int choice;

    //read the option the user typed
    if (scanf("%d", &choice) != 1) {
        return;
    }
    getchar(); 
    //consume newline if this is not here then next fgets breaks

    if (choice == 1) {
        //user wants to search by address
        char name[150], norm[150];
        int  num;
        printf("Enter street name (e.g. \"Carrer de Roc Boronat\"): ");
        if (!fgets(name, sizeof(name), stdin)) return;
        name[strcspn(name, "\r\n")] = 0;
        normalize_name(norm, name);

        printf("Enter street number (e.g. \"138\"): ");
        if (scanf("%d", &num) != 1) {
            return;
        }
        getchar();

        //try to find the exact house in the map
        if (!find_and_return_coords(houses, norm, num, lat, lon)) {

            //check if the street name itself exists
            int street_known = 0;
            House* cur = houses;
            while (cur) {
                char nc[150];
                normalize_name(nc, cur->street_name);
                if (strcmp(nc, norm) == 0) { 
                    street_known = 1; break; 
                }
                cur = cur->next;
            }
            if (street_known) {

                //treet exists but number is wrong
                handle_number(houses, norm, num, lat, lon);
            } else {

                //street name not found offer alternatives
                handle_unknown_street(houses, norm, num, lat, lon);
            }
        } else {
            printf("\n    Found at (%f, %f)\n", *lat, *lon);
        }

    } else if (choice == 2) {
        //user wants to search by place name
        char name[200];
        printf("Enter place name: ");
        if (!fgets(name, sizeof(name), stdin)) return;
        name[strcspn(name, "\r\n")] = 0;

        //find the place, if not found try similar names
        Place* p = find_place_fuzzy(places, name);
        if (p) {
            *lat = p->lat;
            *lon = p->lon;
            printf("\n    Found at (%f, %f)\n", *lat, *lon);
        } else {
            printf("    Place not found.\n");
        }

    } else if (choice == 3) {
        //user types the coordinates directly
        printf("Enter latitude: ");
        if (scanf("%lf", lat) != 1) {
            return;
        }
        printf("Enter longitude: ");
        if (scanf("%lf", lon) != 1) {
            return;
        }
        getchar();
        printf("\n    Using coordinate (%f, %f)\n", *lat, *lon);

    } else {
        printf("    Invalid option.\n");
    }
}


//available maps
static const char* MAPS[] = { "xs_1", "xs_2" };
static const int   N_MAPS = 2;

// show the maps and let the user pick one
static void choose_map(char* map_out) {
    printf("Available maps:\n");
    for (int i = 0; i < N_MAPS; i++)
        printf("  [%d] %s\n", i + 1, MAPS[i]);
    printf("Choose a map (1-%d): ", N_MAPS);
    int choice;
    if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= N_MAPS) {
        strcpy(map_out, MAPS[choice - 1]);
    } else {
        printf("Invalid choice, defaulting to xs_1.\n");
        strcpy(map_out, "xs_1");
    }
    getchar();
}


//print connected streets from a segment
static void print_connections(Street* s) {
    if (!s) {
        return;
    }
    printf("    From this street segment, you can go to:\n");
    Intersection* inter = get_intersection(s->to_id);
    if (!inter) { 
        printf("      (no connections found)\n"); return; 
    }
    Edge* e = inter->connections;
    while (e) {
        printf("      - %s\n", e->street->name);
        //show what that street connects with
        Intersection* inter2 = get_intersection(e->street->to_id);
        if (inter2) {
            Edge* e2 = inter2->connections;
            int printed = 0;
            while (e2) {
                if (!printed) printf("          Which is connected to:\n");
                printf("           - %s\n", e2->street->name);
                printed = 1;
                e2 = e2->next;
            }
        }
        e = e->next;
    }
}


//main
int main(void) {
    char map[50];
    int  h_count = 0, p_count = 0, s_count = 0;

    //let user pick the map
    choose_map(map);

    // load everything
    House*  houses  = load_houses(map,  &h_count);
    Place*  places  = load_places(map,  &p_count);
    Street* streets = load_streets(map, &s_count);

    if (!houses || !places || !streets) {
        fprintf(stderr, "Could not load map '%s'. Run the program from the project root.\n", map);
        free_houses(houses);
        free_places(places);
        free_streets(streets);
        return EXIT_FAILURE;
    }

    printf("%d houses loaded\n",  h_count);
    printf("%d places loaded\n",  p_count);
    printf("%d streets loaded\n", s_count);

    build_intersection_map(streets);

    double lat_f = 0.0, lon_f = 0.0;
    double lat_t = 0.0, lon_t = 0.0;

    // ask for origin
    printf("\n--- ORIGIN ---\n");
    printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
    get_location(houses, places, &lat_f, &lon_f);
    Street* start = find_closest_street(streets, lat_f, lon_f);
    if (start) {
        printf("    Closest street: %s\n", start->name);
        printf("    Between %lld (%.6f, %.6f) and %lld (%.6f, %.6f)\n",
               start->from_id, start->from_lat, start->from_lon,
               start->to_id,   start->to_lat,   start->to_lon);
        print_connections(start);
    }

    //ask for destination
    printf("\n--- DESTINATION ---\n");
    printf("Where do you want to go? Address (1), Place (2) or Coordinate (3)? ");
    get_location(houses, places, &lat_t, &lon_t);
    Street* end = find_closest_street(streets, lat_t, lon_t);
    if (end) {
        printf("    Closest street: %s\n", end->name);
        printf("    Between %lld (%.6f, %.6f) and %lld (%.6f, %.6f)\n",
               end->from_id, end->from_lat, end->from_lon,
               end->to_id,   end->to_lat,   end->to_lon);
    }

    if (!start || !end) {
        fprintf(stderr, "Could not determine a valid route endpoint.\n");
        free_intersection_map();
        free_houses(houses);
        free_places(places);
        free_streets(streets);
        return EXIT_FAILURE;
    }

    printf("\n--- ROUTE ---\n");

    struct timespec t0, t1;
    long long elapsed_ns;
    Street* curr;

    //measure intersection lookup time using hash map
    clock_gettime(CLOCK_MONOTONIC, &t0);
    get_intersection(start->to_id);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    elapsed_ns = (t1.tv_sec - t0.tv_sec) * 1000000000LL + (t1.tv_nsec - t0.tv_nsec);
    printf("Intersection hashmap: %lld ns\n", elapsed_ns);


    // measure intersection lookup time using sequential search
    clock_gettime(CLOCK_MONOTONIC, &t0);
    curr = streets;
    while (curr) {
        if (curr->from_id == start->to_id) {
            break;
        }
        curr = curr->next;
    }


    clock_gettime(CLOCK_MONOTONIC, &t1);
    elapsed_ns = (t1.tv_sec - t0.tv_sec) * 1000000000LL + (t1.tv_nsec - t0.tv_nsec);
    printf("Intersection sequential: %lld ns\n", elapsed_ns);

     //measure bfs time using hash map
    clock_gettime(CLOCK_MONOTONIC, &t0);
    find_route_bfs(start, end);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    elapsed_ns = (t1.tv_sec - t0.tv_sec) * 1000000000LL + (t1.tv_nsec - t0.tv_nsec);
    printf("BFS hashmap: %lld ns\n", elapsed_ns);

    //measure bfs time using sequential search
    clock_gettime(CLOCK_MONOTONIC, &t0);
    find_route_bfs_sequential(start, end, streets);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    elapsed_ns = (t1.tv_sec - t0.tv_sec) * 1000000000LL + (t1.tv_nsec - t0.tv_nsec);
    printf("BFS sequential: %lld ns\n", elapsed_ns);


    //free all loaded data
    free_intersection_map();
    free_houses(houses);
    free_places(places);
    free_streets(streets);
    return 0;
}

