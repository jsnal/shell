CC = gcc
CFLAGS = -Wall -O2 -g -Werror=format-security -fcommon

SOURCES = \
					../src/list.c \
					../src/util.c \
					../src/debug.c \
					../src/hashtable.c \
					./asserts.c \

COMMON_OBJECTS = ${SOURCES:.c=.o}
TESTS = test_list test_hashtable

all: $(TESTS)

$(TESTS): $(SOURCES)

.PHONY: clean
clean:
	rm -f $(TESTS) $(COMMON_OBJECTS)
