#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/places.h"
#include "../src/houses.h"
#include "utils.h"

// dynamically allocates memory for a place node and sets its values
static Place* make_place(const char* id, const char* name,const char* cat, double lat, double lon) {
    
    Place* p = malloc(sizeof(Place));
    strcpy(p->id, id);
    strcpy(p->name, name);
    strcpy(p->category, cat);
    p->lat = lat;
    p->lon = lon;
    p->next = NULL;
    return p;
}

// creates a small linked list with 3 elements for testing.
static Place* build_places_list() {
    Place* p1 = make_place("1", "L'Illa Diagonal",      "mall", 41.389559, 2.135112);
    Place* p2 = make_place("2", "Area Tallers",          "park", 41.380000, 2.160000);
    Place* p3 = make_place("3", "Parc de la Ciutadella", "park", 41.386000, 2.186000);
    p1->next = p2;
    p2->next = p3;
    return p1;
}

// checks that a place can be found using its exact name match
void test_find_place_exact() {
    runningtest("test_find_place_exact");
    {
        Place* list = build_places_list();
        Place* p = find_place_by_name(list, "L'Illa Diagonal");
        assertEqualsInt(p != NULL, 1);
        free_places(list);
    }
    successtest();
}

// verifies that searching a name works regardless of upper/lower case
void test_find_place_case_insensitive() {
    runningtest("test_find_place_case_insensitive");
    {
        Place* list = build_places_list();
        Place* p = find_place_by_name(list, "l'illa diagonal");
        assertEqualsInt(p != NULL, 1);
        free_places(list);
    }
    successtest();
}

// ensures that searching for a non-existent name returns null
void test_find_place_not_found() {
    runningtest("test_find_place_not_found");
    {
        Place* list = build_places_list();
        Place* p = find_place_by_name(list, "Lloc Inexistent");
        assertEqualsInt(p == NULL, 1);
        free_places(list);
    }
    successtest();
}

//checks that the counting function returns 1 for a matching place
void test_count_places_found() {
    runningtest("test_count_places_found");
    {
        Place* list = build_places_list();
        assertEqualsInt(count_places_by_name(list, "L'Illa Diagonal"), 1);
        free_places(list);
    }
    successtest();
}

//checks that the counting function returns 0 if no places match the name
void test_count_places_not_found() {
    runningtest("test_count_places_not_found");
    {
        Place* list = build_places_list();
        assertEqualsInt(count_places_by_name(list, "No Existeix"), 0);
        free_places(list);
    }
    successtest();
}

// checks if getting a place by index 0 works
void test_get_place_by_index_valid() {
    runningtest("test_get_place_by_index_valid");
    {
        Place* list = build_places_list();
        Place* p = get_place_by_index(list, "L'Illa Diagonal", 0);
        assertEqualsInt(p != NULL, 1);
        free_places(list);
    }
    successtest();
}

// checks that an invalid index returns null
void test_get_place_by_index_out_of_range() {
    runningtest("test_get_place_by_index_out_of_range");
    {
        Place* list = build_places_list();
        Place* p = get_place_by_index(list, "L'Illa Diagonal", 99);
        assertEqualsInt(p == NULL, 1);
        free_places(list);
    }
    successtest();
}

//checks that fuzzy search successfully matches an exact input string
void test_fuzzy_finds_exact() {
    runningtest("test_fuzzy_finds_exact");
    {
        Place* list = build_places_list();
        Place* p = find_place_fuzzy(list, "L'Illa Diagonal");
        assertEqualsInt(p != NULL, 1);
        free_places(list);
    }
    successtest();
}

//checks that passing null to free_places is safe and does not crash
void test_free_null_places() {
    runningtest("test_free_null_places");
    {
        free_places(NULL); //must not crash
    }
    successtest();
}

//main entry point that executes all the places test cases
void places_test() {
    running("places_test");
    {
        test_find_place_exact();
        test_find_place_case_insensitive();
        test_find_place_not_found();
        test_count_places_found();
        test_count_places_not_found();
        test_get_place_by_index_valid();
        test_get_place_by_index_out_of_range();
        test_fuzzy_finds_exact();
        test_free_null_places();
    }
    success();
}