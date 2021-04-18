CC=gcc
CFLAGS=-pedantic -Wall

$(N_OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(N_OUT) $(OBJECTS)

all: a b c

a: src/a.c src/get_line/get_line.o
	$(CC) $(CFLAGS) src/a.c src/get_line/get_line.o -o a

b: src/b.c src/get_line/get_line.o
	$(CC) $(CFLAGS) src/b.c src/get_line/get_line.o -o b

c: src/c.c src/get_line/get_line.o
	$(CC) $(CFLAGS) src/c.c src/get_line/get_line.o -o c

src/get_line/get_line.o: src/get_line/get_line.c src/get_line/get_line.h
	$(CC) $(CFLAGS) src/get_line/get_line.c -c -o src/get_line/get_line.o

.PHONY: clean
clean:
	rm -f a b c src/get_line/get_line.o
