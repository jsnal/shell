CC = gcc
CFLAGS = -Wall -g

include ../src/Makefile.sources

COMMON_SOURCES = $(addprefix ../, $(SOURCES))

COMMON_OBJECTS = ${COMMON_SOURCES:.c=.o}
TESTS = test_list test_hashtable test_resize_buffer

all: $(TESTS)

$(TESTS): $(COMMON_OBJECTS)

.PHONY: clean
clean:
	rm -f $(TESTS) $(COMMON_OBJECTS)
