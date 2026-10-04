#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// checks if two strings match exactly and stops on failure
void assertEquals(const char *found, const char *expected) {
  if (strcmp(expected, found) != 0) {
    fprintf(stderr, "\033[0;31m    Expected '%s' but found '%s'\033[0m\n\n",
            expected, found);
    assert(0);
  }
}

// checks if two integers are equal and stops on failure
void assertEqualsInt(int found, int expected) {
  if (expected != found) {
    fprintf(stderr, "\033[0;31m    Expected '%d' but found '%d'\033[0m\n\n", expected, found);
    assert(0);
  }
}

// verifies that a pointer is null and stops if it is not
void assertNull(void *found) {
  if (NULL != found) {
    fprintf(stderr, "\033[0;31m    Expected '%p' but found '%p'\033[0m\n\n", NULL, found);
    assert(0);
  }
}

// prints a green success message for a whole test suite  
void success() { 
  fprintf(stderr, "\033[0;32mPASSED\n\033[0m"); 
}

// shows a message when a test suite starts.
void running(const char *description) {
  fprintf(stderr, "\033[0;36mRunning: %s\033[0m\n", description);
}

//prints a green pass message for an individual test case
void successtest() { 
  fprintf(stderr, "\033[0;32m    PASSED\033[0m\n"); 
}

//prints a cyan message when starting an individual test case
void runningtest(const char *description) {
  fprintf(stderr, "\033[0;36m  - Running: %s\033[0m\n", description);
}

// prints a final message indicating all tests completed successfully
void allsuccess() {
  fprintf(stderr, "\033[0;32m--- ALL TESTS PASSED --- \n\033[0m");
}