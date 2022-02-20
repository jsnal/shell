CC = gcc

NAME = test_shell
CFLAGS = -Wall -O2 -g -Werror=format-security -fcommon -I../src

# Turn of debugging
# CFLAGS += -DDEBUG

SOURCES = \
					../src/list.c \
					../src/util.c \
					../src/debug.c \
					./tester.c \
					./test_list.c \
					./tester_asserts.c \

OBJECTS = ${SOURCES:.c=.o}
# HEADERS = ${SOURCES:.c=.h}

all: $(NAME)

$(NAME): $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(NAME) $(OBJECTS)
