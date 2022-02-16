CC = gcc

NAME = shell
CFLAGS = -Wall -O2 -Werror=format-security -ggdb3 -fcommon

# Turn of debugging
# CFLAGS += -DDEBUG

SOURCES = \
					src/builtin.c \
					src/debug.c \
					src/execute.c \
					src/hashtable.c \
					src/history.c \
					src/line.c \
					src/parse.c \
					src/redirect.c \
					src/shell.c \
					src/tokenize.c \
					src/variable.c \

OBJECTS = src/main.o
OBJECTS += ${SOURCES:.c=.o}
HEADERS = ${SOURCES:.c=.h}

$(NAME): $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(OBJECTS)

refresh:
	find . -name -prune -o \( -name '*.c' -or -name '*.h' -or -name 'Makefile' \) | sort > shell.files

.PHONY: clean
clean:
	rm -f $(NAME) $(OBJECTS)
