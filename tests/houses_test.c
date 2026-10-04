#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/houses.h"
#include "utils.h"

// helper function to allocate memory dynamically for a House node
static House *make_house(const char *street, int number, double lat, double lon)
{
    House *h = malloc(sizeof(House));
    strcpy(h->street_name, street);
    h->house_number = number;
    h->lat = lat;
    h->lon = lon;
    h->next = NULL;
    return h;
}

// verifies that normalize_name converts correctly all the text to lowercase
void test_normalize_lowercase()
{
    runningtest("test_normalize_lowercase");
    {
        char out[150];
        normalize_name(out, "Carrer De Roc Boronat");
        assertEquals(out, "carrer de roc boronat");
    }
    successtest();
}

// verifies that the abreviations of C. to carrer de
void test_normalize_abbrev_c()
{
    runningtest("test_normalize_abbrev_c");
    {
        char out[150];
        normalize_name(out, "C. de Roc Boronat");
        assertEquals(out, "carrer de roc boronat");
    }
    successtest();
}

// verifies that the abreviation of Av. is expande to avinguda
void test_normalize_abbrev_av()
{
    runningtest("test_normalize_abbrev_av");
    {
        char out[150];
        normalize_name(out, "Av. Diagonal");
        assertEquals(out, "avinguda diagonal");
    }
    successtest();
}

// it checks thaht the function elimnates all the newline characters
void test_normalize_strips_newline()
{
    runningtest("test_normalize_strips_newline");
    {
        char out[150];
        normalize_name(out, "Carrer de Test\n");
        assertEquals(out, "carrer de test");
    }
    successtest();
}

// tests Levenshtein with two identical words the distance must be 0
void test_levenshtein_identical()
{
    runningtest("test_levenshtein_identical");
    {
        assertEqualsInt(levenshtein("hello", "hello"), 0);
    }
    successtest();
}

// tests Levenshtein with a single character difference distance = 1 substitution
void test_levenshtein_one_substitution()
{
    runningtest("test_levenshtein_one_substitution");
    {
        assertEqualsInt(levenshtein("kitten", "sitten"), 1);
    }
    successtest();
}

// checks typos on street entries using the Levenshtein metric
void test_levenshtein_street_typo()
{
    runningtest("test_levenshtein_street_typo");
    {
        // Roc Voronat vs Roc Boronat = 1 substitution
        assertEqualsInt(levenshtein("roc voronat", "roc boronat"), 1);
    }
    successtest();
}

// verifies that it finds an existing street and number in the linked list and correctly returns its associated coordinates
void test_find_existing_house()
{
    runningtest("test_find_existing_house");
    {
        House *h1 = make_house("Carrer de Roc Boronat", 138, 41.403981, 2.193255);
        House *h2 = make_house("Carrer de Roc Boronat", 140, 41.404000, 2.193300);
        h1->next = h2;

        double lat, lon;
        assertEqualsInt(find_and_return_coords(h1, "carrer de roc boronat", 138, &lat, &lon), 1);
        free_houses(h1);
    }
    successtest();
}
// checks the behavior when the street exists but the requested number doesn't
void test_find_missing_number()
{
    runningtest("test_find_missing_number");
    {
        House *h = make_house("Carrer de Roc Boronat", 138, 41.0, 2.0);
        double lat, lon;
        assertEqualsInt(find_and_return_coords(h, "carrer de roc boronat", 999, &lat, &lon), 0);
        free_houses(h);
    }
    successtest();
}

// verifies that if the queried street string does not exist anywhere in the structure it safely returns failure
void test_find_missing_street()
{
    runningtest("test_find_missing_street");
    {
        House *h = make_house("Carrer de Roc Boronat", 1, 41.0, 2.0);
        double lat, lon;
        assertEqualsInt(find_and_return_coords(h, "carrer inexistent", 1, &lat, &lon), 0);
        free_houses(h);
    }
    successtest();
}

// checks that passing null to free_houses is completely safe and doesn't crash
void test_free_null_houses()
{
    runningtest("test_free_null_houses");
    {
        free_houses(NULL); // must not crash
    }
    successtest();
}

// runs all the unit tests for the houses module sequentially
void houses_test()
{
    running("houses_test");
    {
        test_normalize_lowercase();
        test_normalize_abbrev_c();
        test_normalize_abbrev_av();
        test_normalize_strips_newline();
        test_levenshtein_identical();
        test_levenshtein_one_substitution();
        test_levenshtein_street_typo();
        test_find_existing_house();
        test_find_missing_number();
        test_find_missing_street();
        test_free_null_houses();
    }
    success();
}