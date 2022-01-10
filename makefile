CC=gcc
CFLAGS=-I -g
DEPS = parser.h pager.h table_operations.h
OBJ = parser.c main.c pager.c test.c table_operations.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
make: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)