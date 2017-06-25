all: dqn_unit_test.cpp
	mkdir -p bin
	g++ -o bin/dqn_unit_test dqn_unit_test.cpp -lm -Wall -Werror -pthread -ggdb
