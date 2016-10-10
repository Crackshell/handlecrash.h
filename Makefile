CXX=g++

all: x64 x86

lz4.o:
	$(CXX) -g -o lz4.o -c lz4.c
lz4.32.o:
	$(CXX) -m32 -g -o lz4.32.o -c lz4.c
lz4hc.o:
	$(CXX) -g -o lz4hc.o -c lz4hc.c
lz4hc.32.o:
	$(CXX) -m32 -g -o lz4hc.32.o -c lz4hc.c

x64: lz4.o lz4hc.o
	$(CXX) -g -o example_64 -I. example.cpp lz4.o lz4hc.o
x86: lz4.32.o lz4hc.32.o
	$(CXX) -m32 -g -o example_32 -I. example.cpp lz4.32.o lz4hc.32.o

clean:
	rm *.o
	rm example_64
	rm example_32
