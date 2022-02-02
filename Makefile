CC = gcc

NAME = shell
CFLAGS = -Wall -O2 -Werror=format-security -ggdb3 -fcommon

# For debug printing only!
# CFLAGS += -DDEBUG

SOURCES = \
					src/builtin.c \
					src/command.c \
					src/debug.c \
					src/execute.c \
					src/history.c \
					src/line.c \
					src/parse.c \
					src/redirect.c \
					src/shell.c \
					src/tokenize.c

OBJECTS = src/main.o
OBJECTS += ${SOURCES:.c=.o}
HEADERS = ${SOURCES:.c=.h}

$(NAME): $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(NAME) $(OBJECTS)
