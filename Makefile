CC = gcc
CFLAGS = -Wall -O2 -g -Werror=format-security -fcommon
NAME = shell

# Turn of debugging
# CFLAGS += -DDEBUG

include src/Makefile.sources

all: $(NAME)

$(NAME): src/main.o $(OBJECTS) $(HEADERS)
	$(CC) -o $@ src/main.o $(OBJECTS)

.PHONY: tests
tests:
	$(MAKE) -C $(MAKECMDGOALS)

refresh:
	find . -name -prune -o \( -name '*.c' -or -name '*.h' -or -name 'Makefile' \) | sort > shell.files

.PHONY: clean
clean:
	rm -f $(NAME) $(OBJECTS)
