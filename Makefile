CXX=g++

all: example

example:
	$(CXX) -g -o example example.cpp

clean:
	rm example
