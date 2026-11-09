CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

.PHONY: all test clean

all: proj2

proj2: proj2.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test: proj2
	./proj2 input.1 FCFS
	./proj2 input.1 SJF
	./proj2 input.1 RR 5
	./proj2 input.2 FCFS
	./proj2 input.2 SJF
	./proj2 input.2 RR 2
	./proj2 input.3 FCFS
	./proj2 input.3 SJF
	./proj2 input.3 RR 2

clean:
	$(RM) proj2
