CC=gcc
CFLAGS=-Wall -std=c99
LIBF=src/lib/first
LIBS=src/lib/second

all: task1 task2 $(LIBF)/libFirst.so $(LIBS)/libSec.so

task1: $(LIBF)/deriv.c $(LIBF)/translation.c $(LIBF)/first.h src/task1.c
	$(CC) $(CFLAGS) $(LIBF)/deriv.c $(LIBF)/translation.c src/task1.c -o task1 -lm

task2: src/task2.c $(LIBF)/libFirst.so $(LIBS)/libSec.so
	$(CC) $(CFLAGS) src/task2.c -ldl -o task2 -lm

$(LIBF)/libFirst.so: $(LIBF)/deriv.c $(LIBF)/translation.c
	$(CC) $(CFLAGS) -shared -fPIC $(LIBF)/deriv.c $(LIBF)/translation.c -o $(LIBF)/libFirst.so -lm

$(LIBS)/libSec.so: $(LIBS)/deriv.c $(LIBS)/translation.c
	$(CC) $(CFLAGS) -shared -fPIC $(LIBS)/deriv.c $(LIBS)/translation.c -o $(LIBS)/libSec.so -lm

clean:
	rm -f task1 task2 $(LIBF)/libFirst.so $(LIBS)/libSec.so
 