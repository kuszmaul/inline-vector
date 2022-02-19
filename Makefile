CXX=clang++ 
CC=$(CXX)
CXXFLAGS=-W -Wall -Werror -std=c++17 -O2 -ggdb

check: test.run
%.run: %
	./$<
	touch $@
test: test.o
test.o: inline-vector.h