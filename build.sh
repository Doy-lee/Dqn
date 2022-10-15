#!/bin/bash

code_dir=${PWD}
mkdir -p Build

pushd Build
    g++ ${code_dir}/dqn_unit_tests.cpp -D DQN_TEST_WITH_MAIN -std=c++17 -o dqn_unit_tests
popd
