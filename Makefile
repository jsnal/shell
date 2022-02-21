CC = gcc

NAME = shell
CFLAGS = -Wall -O2 -g -Werror=format-security -fcommon

# Turn of debugging
# CFLAGS += -DDEBUG

SOURCES = \
					src/builtin.c \
					src/debug.c \
					src/execute.c \
					src/hashtable.c \
					src/list.c \
					src/line.c \
					src/parse.c \
					src/redirect.c \
					src/shell.c \
					src/tokenize.c \
					src/util.c \
					src/variable.c \

# OBJECTS = src/main.o
OBJECTS = ${SOURCES:.c=.o}
HEADERS = ${SOURCES:.c=.h}

all: $(NAME)

$(NAME): src/main.o $(OBJECTS) $(HEADERS)
	$(CC) -o $@ src/main.o $(OBJECTS)

refresh:
	find . -name -prune -o \( -name '*.c' -or -name '*.h' -or -name 'Makefile' \) | sort > shell.files

.PHONY: clean
clean:
	rm -f $(NAME) $(OBJECTS)
