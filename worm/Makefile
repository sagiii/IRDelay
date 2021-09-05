all: main

CC=g++
CPPFLAGS=-std=c++14

.cpp.o:
	$(CC) -c $(CPPFLAGS) $^ -o $@

main: main.o

main2: main2.o

clean:
	rm -f *~ *.o main

