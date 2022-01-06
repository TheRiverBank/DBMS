CC=gcc
CFLAGS=-I -g
DEPS = parser.h
OBJ = parser.c bored.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
make: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)