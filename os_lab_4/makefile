CC=gcc
OBJECTS = src/laba_4.o src/get_line/get_line.o
CFLAGS=-pedantic -Wall -lpthread -pthread
N_OUT=laba_4

$(N_OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(N_OUT) $(OBJECTS)

src/laba_4.o: src/laba_4.c src/get_line/get_line.o
src/get_line/get_line.o: src/get_line/get_line.c src/get_line/get_line.h

.PHONY: clean
clean:
	-rm $(N_OUT) $(OBJECTS)