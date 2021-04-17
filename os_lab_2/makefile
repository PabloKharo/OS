CC=gcc
OBJECTS = src/laba_2.o src/child/child.o src/get_line/get_line.o 
CFLAGS=-pedantic -Wall
N_OUT=laba_2

$(N_OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(N_OUT) $(OBJECTS)

src/laba_2.o: src/laba_2.c src/child/child.h src/get_line/get_line.h
src/child/child.o: src/child/child.c
src/get_line/get_line.o: src/get_line/get_line.c

.PHONY: clean
clean:
	-rm $(N_OUT) $(OBJECTS)