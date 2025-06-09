CXX = g++
CXXFLAGS = -Wall -g -std=c++11 -I../src

all: test_fs

test_fs: test_fs.cpp ../src/fs.cc gtest/gtest.h
	$(CXX) $(CXXFLAGS) -o $@ test_fs.cpp ../src/fs.cc

check: test_fs
	./test_fs

clean:
	rm -f test_fs *.o

.PHONY: all check clean
