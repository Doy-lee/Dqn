all: dqn_unit_test.cpp
	mkdir -p bin
	gcc -o bin/dqn_unit_test dqn_unit_test.cpp -lm
