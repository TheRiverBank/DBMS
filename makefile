CC=gcc
CFLAGS=-I -g
DEPS = parser.h pager.h table_operations.h sequential_index_file.h
OBJ = parser.c main.c pager.c test.c table_operations.c sequential_index_file.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
make: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)