CC=gcc
CFLAGS=-I -g
DEPS = parser.h pager.h
OBJ = parser.c bored.c pager.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
make: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)