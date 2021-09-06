all: test

CC=g++
CPPFLAGS=-std=c++14

.cpp.o:
	$(CC) -c $(CPPFLAGS) $^ -o $@

test: test.o

clean:
	rm -f *~ *.o test

