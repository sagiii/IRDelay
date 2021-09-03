all: main

CC=g++
CPPFLAGS=-std=c++14

.cpp.o:
	$(CC) -c $(CPPFLAGS) $^ -o $@

main: main.o

clean:
	rm -f *~ *.o main

