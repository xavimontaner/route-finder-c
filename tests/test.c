#include "houses_test.c"
#include "places_test.c"
#include "streets_test.c"
#include "utils.h"

int main(void) {
    houses_test();
    places_test();
    streets_test();
    allsuccess();
    return 0;
}