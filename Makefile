CXX=g++

all: x64 x86

x64:
	$(CXX) -g -o example_64 example.cpp
x86:
	$(CXX) -m32 -g -o example_32 example.cpp

clean:
	rm example_64
	rm example_32
