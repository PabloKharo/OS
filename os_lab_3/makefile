CC=gcc
OBJECTS=src/laba_3.o src/get_int/get_int.o
CFLAGS=-pedantic -Wall -pthread
N_OUT=laba_3

$(N_OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(N_OUT) $(OBJECTS)

src/laba_3.o: src/laba_3.c src/get_int/get_int.h
src/get_int/get_int.o: src/get_int/get_int.c

.PHONY: clean
clean:
	-rm $(N_OUT) $(OBJECTS)
