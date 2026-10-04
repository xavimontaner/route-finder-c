#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/streets.h"
#include "utils.h"

//allocates memory for a street structure and fills its details
static Street* make_street(long long fid, double flat, double flon,long long tid, double tlat, double tlon,double len, const char* name) {
    Street* s = malloc(sizeof(Street));
    s->from_id = fid; s->from_lat = flat; s->from_lon = flon;
    s->to_id   = tid; s->to_lat   = tlat; s->to_lon   = tlon;
    s->length  = len;
    strcpy(s->name, name);
    s->next = NULL;
    return s;
}

// creates a basic 3-street list to simulate a small grahp
static Street* build_streets_list() {
    Street* s1 = make_street(1, 41.400, 2.190, 2, 41.401, 2.191, 150.0, "Carrer A");
    Street* s2 = make_street(2, 41.401, 2.191, 3, 41.402, 2.192, 200.0, "Carrer B");
    Street* s3 = make_street(2, 41.401, 2.191, 4, 41.403, 2.185, 300.0, "Carrer C");
    s1->next = s2;
    s2->next = s3;
    return s1;
}

// checks if the function finds the correct closest street to a coordinate
void test_closest_street() {
    runningtest("test_closest_street");
    {
        Street* list = build_streets_list();
        //midpoint of s1  (41.4005, 2.1905), query right next to it
        Street* c = find_closest_street(list, 41.4005, 2.1905);
        assertEqualsInt(c != NULL, 1);
        assertEqualsInt((int)c->from_id, 1);
        free_streets(list);
    }
    successtest();
}

// tests finding the closest street when the list has only one element
void test_closest_street_single() {
    runningtest("test_closest_street_single");
    {
        Street* s = make_street(10, 41.38, 2.17, 11, 41.381, 2.171, 100.0, "Solo");
        Street* c = find_closest_street(s, 41.38, 2.17);
        assertEqualsInt(c == s, 1);
        free_streets(s);
    }
    successtest();
}

// verifies that intersections are properly generated in the map structure
void test_intersection_nodes_created() {
    runningtest("test_intersection_nodes_created");
    {   Street* list = build_streets_list();
        build_intersection_map(list);
        assertEqualsInt(get_intersection(1) != NULL, 1);
        assertEqualsInt(get_intersection(2) != NULL, 1);
        free_streets(list);
    }
    successtest();
}

// counts the outgoing connections of an intersection to verify graph setup
void test_intersection_edge_count() {
    runningtest("test_intersection_edge_count");
    {   Street* list = build_streets_list();
        build_intersection_map(list);
        //node 2 has s2 and s3 leaving it 2 edges
        Intersection* i2 = get_intersection(2);
        assertEqualsInt(i2 != NULL, 1);
        int edges = 0;
        for (Edge* e = i2->connections; e; e = e->next) edges++;
        assertEqualsInt(edges, 2);
        free_streets(list);
    }
    successtest();
}

// ensures searching for a missing intersection returns null safely
void test_intersection_missing() {
    runningtest("test_intersection_missing");
    {   Street* empty = NULL;
        build_intersection_map(empty);
        assertEqualsInt(get_intersection(99999) == NULL, 1);
    }
    successtest();
}

// verifies that the connected street names are accurately mapped
void test_intersection_correct_name() {
    runningtest("test_intersection_correct_name");
    {   Street* list = build_streets_list();
        build_intersection_map(list);
        Intersection* i1 = get_intersection(1);
        assertEqualsInt(i1 != NULL, 1);
        assertEquals(i1->connections->street->name, "Carrer A");
        free_streets(list);
    }
    successtest();
}

// checks that passing null to free_streets does not crash
void test_free_null_streets() {
    runningtest("test_free_null_streets");
    {
        free_streets(NULL); 
    }
    successtest();
}

//main entry point that executes all the streets test cases
void streets_test() {
    running("streets_test");
    {
        test_closest_street();
        test_closest_street_single();
        test_intersection_nodes_created();
        test_intersection_edge_count();
        test_intersection_missing();
        test_intersection_correct_name();
        test_free_null_streets();
    }
    success();
}