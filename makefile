all: dqn_unit_test.cpp
	mkdir -p bin
	g++ -std=c++14 -o bin/dqn_unit_test dqn_unit_test.cpp -lm -Wall -pthread -ggdb
