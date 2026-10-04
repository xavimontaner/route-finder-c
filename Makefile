CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
LDLIBS = -lm

SOURCES = src/houses.c src/pathfinding.c src/places.c src/streets.c
HEADERS = src/houses.h src/places.h src/streets.h
TARGET = route-finder
TEST_TARGET = tests/run-tests

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): src/main.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) src/main.c $(SOURCES) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

$(TEST_TARGET): tests/test.c tests/utils.c tests/utils.h $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) tests/test.c tests/utils.c $(SOURCES) -o $(TEST_TARGET) $(LDLIBS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
ifeq ($(OS),Windows_NT)
	-del /Q $(TARGET) $(TARGET).exe $(subst /,\\,$(TEST_TARGET)) $(subst /,\\,$(TEST_TARGET)).exe 2>NUL
else
	rm -f $(TARGET) $(TARGET).exe $(TEST_TARGET) $(TEST_TARGET).exe
endif