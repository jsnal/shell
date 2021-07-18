CC = gcc

NAME = shell
CFLAGS = -Wall -O2 -Werror=format-security -ggdb3 -fcommon

# For debug printing only!
# CFLAGS += -DDEBUG

SOURCES = \
					src/builtin.c \
					src/command.c \
					src/tokenize.c \
					src/execute.c \
					src/history.c \
					src/main.c \
					src/parse.c \
					src/redirect.c \
					src/shell.c

OBJECTS = ${SOURCES:.c=.o}

$(NAME): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(NAME) $(OBJECTS)
