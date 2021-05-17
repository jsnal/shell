CC = gcc

NAME = shell
CFLAGS = -Wall -O2 -Werror=format-security -ggdb3

SOURCES = \
					src/main.c \
					src/builtin.c \
					src/exec.c \
					src/command.c \
					src/shell.c

OBJECTS = ${SOURCES:.c=.o}

$(NAME): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(NAME) $(OBJECTS)
