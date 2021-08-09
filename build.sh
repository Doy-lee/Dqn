#!/bin/bash

code_dir=${PWD}
mkdir -p Build

pushd Build
    g++ ${code_dir}/Dqn_Tests.cpp -D DQN_TEST_WITH_MAIN -std=c++17 -o Dqn_UnitTests
popd
